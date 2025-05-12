#pragma once

#include "rqobject.h"
#include "rqbasesession.h"

class RQTcpClient : public RQObject, public RQBaseSession
{
public:
    using PTR = std::shared_ptr<RQTcpClient>;

public:
    using OnClose = std::function<void()>;

public:
    RQTcpClient();
    ~RQTcpClient();

public:
    void Start(const std::string& ip, int port, OnClose onclose = nullptr);
    void Stop();
    void ReStart();

private:
    int Input(RQMsg::PTR msg);
    int TcpConnected(RQMsg::PTR msg);
    int TcpDisconnected(RQMsg::PTR msg);

protected:
    virtual void OnConnected(int64_t uuid) = 0;
    virtual void OnDisconnected() = 0;
    
protected:
    virtual void Recved(const char* data, size_t size) = 0;

protected:
    void Send(const char* data, size_t size);
    void Send(const std::string& data);

protected:
    std::string m_ip;
    int m_port = -1;

    OnClose m_close;

    int64_t m_allRecved = 0;
    int64_t m_allSended = 0;
};

