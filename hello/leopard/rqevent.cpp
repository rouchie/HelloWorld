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

RQEvent::PTR RQEvent::Inst()
{
    static RQEvent::PTR inst = std::make_shared<RQEvent>();
    return inst;
}

void RQEvent::PipeEvent(evutil_socket_t fd, short, void* arg)
{
    cid_t cid;
    auto msg = RQEvent::Inst()->ReadPipe(cid);
    if (!msg || msg->Command() != cid) {
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

    struct bufferevent* bufev = bufferevent_socket_new(RQEvent::Inst()->Base(), fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bufev) {
        THROW("bufferevent_socket_new failed");
    }

    nlohmann::json js;
    js["IP"] = ip;
    js["PORT"] = sin->sin_port;

    RQTcpMsg::PTR listener = std::static_pointer_cast<RQTcpMsg>(RQEvent::Inst()->FindMsg(arg));

    RQBaseSession::PTR session = listener->Session()((int64_t) bufev);
    RQEvent::Inst()->BindSession(session);

    bufferevent_setcb(bufev, Read, nullptr, Event, session.get());
    bufferevent_enable(bufev, EV_READ|EV_PERSIST);

    SPDLOG_INFO("lister: {:X}", (int64_t) session.get());
}

void RQEvent::Read(struct bufferevent* bev, void* ctx)
{
	struct evbuffer* pInput = bufferevent_get_input(bev);
	size_t s = evbuffer_get_length(pInput);

    std::string binary;
    binary.resize(s);
	evbuffer_remove(pInput, (void*) binary.data(), s);

    RQBaseSession::PTR session = RQEvent::Inst()->FindSession(ctx);
    REQ(session->Id(), RQMsg::Builder(IPC_COMMAND_ID_TCP_READ)->Bin(binary));

	//static int64_t ii = 0;
	//SPDLOG_INFO("Read Len[{:06d}] {:06d}", s, ++ii);
}

void RQEvent::Event(struct bufferevent* bev, short what, void* ctx)
{
    RQBaseSession::PTR session = RQEvent::Inst()->FindSession(ctx);

	if (what & BEV_EVENT_ERROR || what & BEV_EVENT_EOF) {
        RQEvent::Inst()->UnBindSession(ctx);
    }
    else if (what & BEV_EVENT_CONNECTED) {
    }
}

void RQEvent::Cli_Read(struct bufferevent* bev, void* ctx)
{
	struct evbuffer* pInput = bufferevent_get_input(bev);
	size_t s = evbuffer_get_length(pInput);

    std::string binary;
    binary.resize(s);
	evbuffer_remove(pInput, (void*) binary.data(), s);

    RQBaseSession::PTR session = RQEvent::Inst()->FindSession(ctx);
    REQ(session->Id(), RQMsg::Builder(IPC_COMMAND_ID_TCP_READ)->Bin(binary));
}

void RQEvent::Cli_Event(struct bufferevent* bev, short what, void* ctx)
{
    RQBaseSession::PTR session = RQEvent::Inst()->FindSession(ctx);

	if (what & BEV_EVENT_EOF) {
        REQ(session->Id(), RQMsg::Builder(IPC_COMMAND_ID_TCP_DISCONNECT)->Pam(what));
    }
    else if (what & BEV_EVENT_ERROR) {
        REQ(session->Id(), RQMsg::Builder(IPC_COMMAND_ID_TCP_DISCONNECT)->Pam(what));
    }
    else if (what & BEV_EVENT_CONNECTED) {
		bufferevent_enable(bev, EV_READ | EV_WRITE);
        REQ(session->Id(), RQMsg::Builder(IPC_COMMAND_ID_TCP_CONNECT)->Pam(session->UUID())->Num((int64_t)ctx));
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
    auto msg = std::make_shared<RQTcpMsg>(mid, mid, cid, port);
    msg->BindSession(session);

    WritePipe(msg);
}

void RQEvent::PipeTcpSend(mid_t mid, int64_t session, const char* data, size_t size)
{
    auto msg = RQMsg::Builder(mid, IPC_COMMAND_ID_TCP_SEND)->Num(session)->Bin(std::string(data, size));

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

void RQEvent::WritePipe(RQMsg::PTR msg)
{
    cid_t cid = msg->Command();

    std::lock_guard<std::mutex> lock(m_mtx);
    m_pipeMsg.push_back(msg);
    ::send(m_sockets[1], (char*)&cid, sizeof(cid_t), 0);
}

RQMsg::PTR RQEvent::ReadPipe(cid_t& cid)
{
    std::lock_guard<std::mutex> lock(m_mtx);
	::recv(m_sockets[0], (char*)&cid, sizeof(cid_t), 0);
    auto msg = m_pipeMsg.front();
    m_pipeMsg.pop_front();
    return msg;
}

void RQEvent::TcpListen(RQMsg::PTR msg)
{
    int port = msg->Number();

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(struct sockaddr_in));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    auto listener = evconnlistener_new_bind(RQEvent::Inst()->Base(), RQEvent::Listener, msg.get(), LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
    if (!listener) {
        THROW("bind port[{}] failed", port);
    }

    RQEvent::Inst()->BindMsg(msg);

    REQ(msg->Sender(), RQMsg::Builder(msg->Command())->Num((int64_t) msg.get()));
}

void RQEvent::TcpSend(RQMsg::PTR msg)
{
    struct bufferevent *bufev = (struct bufferevent *) msg->Number();
    const std::string& binary = msg->Binary();
	if (bufferevent_write(bufev, binary.data(), binary.size()) != 0) {
		// TODO: 发送错误
	}
}

void RQEvent::TcpConnect(RQMsg::PTR msg)
{
    int port = msg->Num();
    const std::string ip = msg->Msg();

	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &sin.sin_addr);

    bufferevent* bfevent = bufferevent_socket_new(RQEvent::Inst()->Base(), -1, BEV_OPT_CLOSE_ON_FREE);

    RQBaseSession::PTR session = std::make_shared<RQBaseSession>(msg->Snd(), (int64_t)bfevent);

    bufferevent_setcb(bfevent, Cli_Read, nullptr, Cli_Event, session.get());

	if (bufferevent_socket_connect(bfevent, (struct sockaddr*)&sin, sizeof(sin))) {
        RSP(msg, -1);
	}

    RQEvent::Inst()->BindSession(session);
}

void RQEvent::TcpDisconnect(RQMsg::PTR msg)
{
    RQBaseSession::PTR session = RQEvent::Inst()->FindSession((void*)msg->Pam());
    if (!session) {
        return;
    }

    struct bufferevent *bufev = (struct bufferevent *) session->UUID();
    bufferevent_free(bufev);

    RQEvent::Inst()->UnBindSession((void*)msg->Number());
}

void RQEvent::BindMsg(RQMsg::PTR msg)
{
    m_bindMsg.emplace(msg.get(), msg);
}

RQMsg::PTR RQEvent::FindMsg(void* arg)
{
    auto it = m_bindMsg.find(arg);
    if (it == m_bindMsg.end()) {
        return nullptr;
    }
    return it->second;
}

void RQEvent::BindSession(RQBaseSession::PTR session)
{
    m_bindSession.emplace(session.get(), session);
}

void RQEvent::UnBindSession(void* arg)
{
    m_bindSession.erase(arg);
}

RQBaseSession::PTR RQEvent::FindSession(void* arg)
{
    auto it = m_bindSession.find(arg);
    if (it == m_bindSession.end()) {
        return nullptr;
    }
    return it->second;
}

struct event_base* RQEvent::Base()
{
    return m_base;
}
