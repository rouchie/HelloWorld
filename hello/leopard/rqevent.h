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

class RQEvent : virtual public std::enable_shared_from_this<RQEvent>
{
public:
    using PTR = std::shared_ptr<RQEvent>;

public:
    static PTR Inst();

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
    void WritePipe(RQMsg::PTR msg);
    RQMsg::PTR ReadPipe(cid_t& cid);

public:
    void TcpListen(RQMsg::PTR msg);
    void TcpSend(RQMsg::PTR msg);

public:
    void TcpConnect(RQMsg::PTR msg);
    void TcpDisconnect(RQMsg::PTR msg);

public:
    void BindMsg(RQMsg::PTR msg);
    RQMsg::PTR FindMsg(void* arg);

public:
    void BindSession(RQBaseSession::PTR session);
    void UnBindSession(void* arg);
    RQBaseSession::PTR FindSession(void* arg);

public:
    struct event_base* Base();

private:
    evutil_socket_t m_sockets[2];
    struct event_base* m_base = nullptr;
    struct event* m_pipe = nullptr;

private:
    std::mutex m_mtx;
    std::list<RQMsg::PTR> m_pipeMsg;
    std::unordered_map<void*, RQMsg::PTR> m_bindMsg;
    std::unordered_map<void*, RQBaseSession::PTR> m_bindSession;
};
