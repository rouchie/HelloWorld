#pragma once

#include "rqobject.h"
#include "rqbasesession.h"

class RQTcpClient : public RQObject, public RQBaseSession
{
public:
    using Ptr = std::shared_ptr<RQTcpClient>;

public:
    using OnClose = std::function<void(int64_t)>;

public:
    RQTcpClient();
    ~RQTcpClient() override;

public:
    void Start(const std::string& ip, int port, OnClose onclose = nullptr);
    void Stop() const;
    void ReStart();

private:
    int Input(const RQMsg::Ptr& msg);
    int TcpConnected(const RQMsg::Ptr& msg);
    int TcpDisconnected(const RQMsg::Ptr& msg);

protected:
    virtual void OnConnected(int64_t uuid) = 0;
    virtual void OnDisconnected(int64_t code) = 0;
    
protected:
    virtual void OnRecved(const char* data, size_t size) = 0;

protected:
    void Send(const char* data, size_t size);
    void Send(const std::string& data);

protected:
    std::string m_ip;
    int m_port = -1;

    OnClose m_close;

    size_t m_allRecved = 0;
    size_t m_allSended = 0;
};

