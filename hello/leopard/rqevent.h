#pragma once

#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>

#include "rqdefine.h"
#include "rqmsg.h"
#include "rqtcpsession.h"

#include "event2/event.h"
#include "event2/listener.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"

class RQEventSession : public std::enable_shared_from_this<RQEventSession>
{
public:
    using Ptr = std::shared_ptr<RQEventSession>;

public:
    static Ptr MakeInst();

public:
    RQEventSession() = default;

public:
    int64_t uuid = 0;

public:
    bufferevent* bf_event = nullptr;
    mid_t recver = 0;

public:
    RQObject::PTR object;
};

class RQEvent : virtual public std::enable_shared_from_this<RQEvent>
{
public:
    using Ptr = std::shared_ptr<RQEvent>;

public:
    static Ptr Inst();

public:
    static void PipeEvent(evutil_socket_t fd, short, void* arg);
    static void Listener(struct evconnlistener*, evutil_socket_t fd, struct sockaddr* addr, int socklen, void* arg);
    static void Read(struct bufferevent *bev, void *ctx);
    static void Event(struct bufferevent *bev, short what, void *ctx);

    static void Cli_Read(struct bufferevent *bev, void *ctx);
    static void Cli_Event(struct bufferevent *bev, short what, void *ctx);

public:
    RQEvent();

public:
    bool Start();

public:
    void PipeAddListen(mid_t mid, int port, std::function<RQTcpSession::PTR(int64_t)> session);
    void PipeTcpSend(mid_t mid, int64_t session, const char* data, size_t size);

public:
    void PipeAddClient(mid_t mid, const std::string& ip, int port);
    void PipeDelClient(mid_t mid, int64_t uuid);

public:
    void WritePipe(RQMsg::Ptr msg);
    RQMsg::Ptr ReadPipe(cid_t& cid);

public:
    void TcpListen(RQMsg::Ptr msg);
    void TcpSend(RQMsg::Ptr msg);

public:
    void TcpConnect(RQMsg::Ptr msg);
    void TcpDisconnect(RQMsg::Ptr msg);

public:
    void BindMsg(RQMsg::Ptr msg);
    RQMsg::Ptr FindMsg(void* arg);

public:
    void BindSession(const RQEventSession::Ptr& session);
    void UnBindSession(void* arg);
    RQEventSession::Ptr FindSession(void* arg);

public:
    struct event_base* Base() const;

private:
    evutil_socket_t m_sockets[2]{};
    struct event_base* m_base = nullptr;
    struct event* m_pipe = nullptr;

private:
    std::mutex m_mtx;
    std::list<RQMsg::Ptr> m_pipeMsg;
    std::unordered_map<void*, RQMsg::Ptr> m_bindMsg;
    std::unordered_map<void*, RQEventSession::Ptr> m_bindSession;
};
