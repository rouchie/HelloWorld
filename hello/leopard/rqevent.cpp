#include "rqevent.h"
#include "rqcoremanager.h"

#include "utils/UtilsSpdlog.h"
#include "utils/UtilsJson.h"

#include "workflow/WFTaskFactory.h"

class RQTcpMsg : public RQMsg
{
public:
    using PTR = std::shared_ptr<RQTcpMsg>;

public:
    using RQMsg::RQMsg;

public:
    void BindSession(std::function<RQTcpSession::PTR(int64_t)> session) { m_session = session; }
    std::function<RQTcpSession::PTR(int64_t)> Session() { return m_session; }

private:
    std::function<RQTcpSession::PTR(int64_t)> m_session;
};

RQEventSession::Ptr RQEventSession::MakeInst()
{
    RQEventSession::Ptr session = std::make_shared<RQEventSession>();
    session->uuid = (int64_t) session.get();
    return session;
}

RQEvent::Ptr RQEvent::Inst()
{
    static RQEvent::Ptr inst = std::make_shared<RQEvent>();
    return inst;
}

void RQEvent::PipeEvent(evutil_socket_t fd, short, void* arg)
{
    cid_t cid;
    auto msg = RQEvent::Inst()->ReadPipe(cid);
    if (!msg || msg->Cmd() != cid) {
        // 不可能发生
        return;
    }

    switch (cid)
    {
    case IPC_COMMAND_ID_TCP_LISTEN:
    {
        RQEvent::Inst()->TcpListen(msg);
    } break;
    case IPC_COMMAND_ID_TCP_SEND:
    {
        RQEvent::Inst()->TcpSend(msg);
    } break;
    case IPC_COMMAND_ID_TCP_ADD_CLIENT:
    {
        RQEvent::Inst()->TcpConnect(msg);
    } break;
    case IPC_COMMAND_ID_TCP_DEL_CLIENT:
    {
        RQEvent::Inst()->TcpDisconnect(msg);
    } break;
    default:
        break;
    }
}

void RQEvent::Listener(struct evconnlistener*, evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg)
{
    struct sockaddr_in* sin = (sockaddr_in*)addr;
    char ip[32] = { 0 };
    inet_ntop(AF_INET, &sin->sin_addr, ip, sizeof(ip));

    struct bufferevent* bf_event = bufferevent_socket_new(RQEvent::Inst()->Base(), fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bf_event) {
        THROW("bufferevent_socket_new failed");
    }

    nlohmann::json js;
    js["IP"] = ip;
    js["PORT"] = sin->sin_port;

    const RQTcpMsg::PTR listener = std::static_pointer_cast<RQTcpMsg>(RQEvent::Inst()->FindMsg(arg));

    const RQEventSession::Ptr session = RQEventSession::MakeInst();
    const RQObject::PTR object = listener->Session()(session->uuid);

    session->bf_event = bf_event;
    session->recver = object->Mid();
    session->object = object;

    RQEvent::Inst()->BindSession(session);

    bufferevent_setcb(bf_event, Read, nullptr, Event, session.get());
    bufferevent_enable(bf_event, EV_READ|EV_PERSIST);

    SPDLOG_INFO("lister: {:X}", session->uuid);
}

void RQEvent::Read(struct bufferevent* bev, void* ctx)
{
	struct evbuffer* pInput = bufferevent_get_input(bev);
	const size_t s = evbuffer_get_length(pInput);

    std::string binary;
    binary.resize(s);
	evbuffer_remove(pInput, (void*) binary.data(), s);

    const RQEventSession::Ptr session = RQEvent::Inst()->FindSession(ctx);
    REQ(session->recver, RQMsg::Builder(IPC_COMMAND_ID_TCP_READ)->Bin(binary));
}

void RQEvent::Event(struct bufferevent* bev, short what, void* ctx)
{
    RQEventSession::Ptr session = RQEvent::Inst()->FindSession(ctx);

	if (what & BEV_EVENT_ERROR || what & BEV_EVENT_EOF) {
        REQ(session->recver, RQMsg::Builder(IPC_COMMAND_ID_TCP_DISCONNECT)->Pam(what));
        RQEvent::Inst()->UnBindSession(ctx);
    }
    else if (what & BEV_EVENT_CONNECTED) {
		bufferevent_enable(bev, EV_READ | EV_WRITE);
        REQ(session->recver, RQMsg::Builder(IPC_COMMAND_ID_TCP_CONNECT)->Pam(session->uuid));
    }
}

void RQEvent::Cli_Read(struct bufferevent* bev, void* ctx)
{
	struct evbuffer* pInput = bufferevent_get_input(bev);
	size_t s = evbuffer_get_length(pInput);

    std::string binary;
    binary.resize(s);
	evbuffer_remove(pInput, (void*) binary.data(), s);

    RQEventSession::Ptr session = RQEvent::Inst()->FindSession(ctx);
    REQ(session->recver, RQMsg::Builder(IPC_COMMAND_ID_TCP_READ)->Bin(binary));
}

void RQEvent::Cli_Event(struct bufferevent* bev, short what, void* ctx)
{
    RQEventSession::Ptr session = RQEvent::Inst()->FindSession(ctx);

	if (what & BEV_EVENT_ERROR || what & BEV_EVENT_EOF) {
        REQ(session->recver, RQMsg::Builder(IPC_COMMAND_ID_TCP_DISCONNECT)->Pam(what));
        RQEvent::Inst()->UnBindSession(ctx);
    }
    else if (what & BEV_EVENT_CONNECTED) {
		bufferevent_enable(bev, EV_READ | EV_WRITE);
        REQ(session->recver, RQMsg::Builder(IPC_COMMAND_ID_TCP_CONNECT)->Pam(session->uuid));
    }
}

RQEvent::RQEvent()
{
    memset(m_sockets, 0, sizeof(m_sockets));
}

bool RQEvent::Start()
{
    evutil_socketpair(AF_INET, SOCK_STREAM, 0, m_sockets);

    m_base = event_base_new();
    if (!m_base) {
        THROW("event base new failed");
    }

    // pipe事件添加
    m_pipe = event_new(m_base, m_sockets[0], EV_READ | EV_PERSIST, PipeEvent, nullptr);
    if (!m_pipe) {
        THROW("event new failed");
    }

    int nRet = event_add(m_pipe, nullptr);
    if (nRet < 0) {
        THROW("event add failed");
    }

	auto* task = WFTaskFactory::create_go_task("libevent", event_base_dispatch, m_base);
	task->start();

	return true;
}

void RQEvent::PipeAddListen(mid_t mid, int port, std::function<RQTcpSession::PTR(int64_t)> session)
{
    cid_t cid = IPC_COMMAND_ID_TCP_LISTEN;
    auto msg = std::make_shared<RQTcpMsg>(mid, cid, port);
    msg->BindSession(std::move(session));
    WritePipe(msg);
}

void RQEvent::PipeTcpSend(mid_t mid, int64_t session, const char* data, size_t size)
{
    auto msg = RQMsg::Builder(mid, IPC_COMMAND_ID_TCP_SEND)->Pam(session)->Bin(std::string(data, size));
    WritePipe(msg);
}

void RQEvent::PipeAddClient(mid_t mid, const std::string& ip, int port)
{
    auto msg = RQMsg::Builder(mid, IPC_COMMAND_ID_TCP_ADD_CLIENT)->Msg(ip)->Num(port);
    WritePipe(msg);
}

void RQEvent::PipeDelClient(mid_t mid, int64_t uuid)
{
    auto msg = RQMsg::Builder(mid, IPC_COMMAND_ID_TCP_DEL_CLIENT)->Pam(uuid);
    WritePipe(msg);
}

void RQEvent::WritePipe(RQMsg::Ptr msg)
{
    cid_t cid = msg->Cmd();

    std::lock_guard<std::mutex> lock(m_mtx);
    m_pipeMsg.push_back(msg);
    ::send(m_sockets[1], (char*)&cid, sizeof(cid_t), 0);
}

RQMsg::Ptr RQEvent::ReadPipe(cid_t& cid)
{
    std::lock_guard<std::mutex> lock(m_mtx);
	::recv(m_sockets[0], (char*)&cid, sizeof(cid_t), 0);
    auto msg = m_pipeMsg.front();
    m_pipeMsg.pop_front();
    return msg;
}

void RQEvent::TcpListen(RQMsg::Ptr msg)
{
    int port = msg->Num();

    struct sockaddr_in sin{};
    memset(&sin, 0, sizeof(struct sockaddr_in));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    auto listener = evconnlistener_new_bind(RQEvent::Inst()->Base(), RQEvent::Listener, msg.get(), LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
    if (!listener) {
        THROW("bind port[{}] failed", port);
    }

    RQEvent::Inst()->BindMsg(msg);

    REQ(msg->Snd(), RQMsg::Builder(msg->Cmd())->Num((int64_t) msg.get()));
}

void RQEvent::TcpSend(RQMsg::Ptr msg)
{
    RQEventSession::Ptr session = RQEvent::Inst()->FindSession((void*)msg->Pam());
    if (!session) {
        return;
    }

    struct bufferevent *bf_event = session->bf_event;
    const std::string& binary = msg->Bin();
	if (bufferevent_write(bf_event, binary.data(), binary.size()) != 0) {
		// TODO: 发送错误
	}
}

void RQEvent::TcpConnect(RQMsg::Ptr msg)
{
    int port = msg->Num();
    const std::string ip = msg->Msg();

	struct sockaddr_in sin{};

	memset(&sin, 0, sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &sin.sin_addr);

    bufferevent* bf_event = bufferevent_socket_new(RQEvent::Inst()->Base(), -1, BEV_OPT_CLOSE_ON_FREE);

    RQEventSession::Ptr session = RQEventSession::MakeInst();
    session->recver = msg->Snd();
    session->bf_event = bf_event;

    bufferevent_setcb(bf_event, Read, nullptr, Event, session.get());

	if (bufferevent_socket_connect(bf_event, reinterpret_cast<struct sockaddr *>(&sin), sizeof(sin))) {
        RSP(msg, -1);
	}

    RQEvent::Inst()->BindSession(session);
}

void RQEvent::TcpDisconnect(RQMsg::Ptr msg)
{
    RQEventSession::Ptr session = RQEvent::Inst()->FindSession((void*)msg->Pam());
    if (!session) {
        return;
    }

    struct bufferevent *bf_event = session->bf_event;
    bufferevent_free(bf_event);

    RQEvent::Inst()->UnBindSession(session.get());
}

void RQEvent::BindMsg(RQMsg::Ptr msg)
{
    m_bindMsg.emplace(msg.get(), msg);
}

RQMsg::Ptr RQEvent::FindMsg(void* arg)
{
    auto it = m_bindMsg.find(arg);
    if (it == m_bindMsg.end()) {
        return nullptr;
    }
    return it->second;
}

void RQEvent::BindSession(const RQEventSession::Ptr& session)
{
    m_bindSession.emplace(session.get(), session);
}

void RQEvent::UnBindSession(void* arg)
{
    m_bindSession.erase(arg);
}

RQEventSession::Ptr RQEvent::FindSession(void* arg)
{
    const auto it = m_bindSession.find(arg);
    if (it == m_bindSession.end()) {
        return nullptr;
    }
    return it->second;
}

struct event_base* RQEvent::Base() const
{
    return m_base;
}
