#pragma once

#include "rqobject.h"
#include "rqbasesession.h"

#include "workflow/RWLock.h"

#include <map>

class RQTcpSession : public RQObject, public RQBaseSession
{
public:
    using PTR = std::shared_ptr<RQTcpSession>;

public:
    explicit RQTcpSession(int64_t uuid);
    ~RQTcpSession() override;

private:
    int Input(RQMsg::Ptr msg);

protected:
    void Started() override;

protected:
    virtual void Recved(const char* data, size_t size) = 0;

protected:
    void Send(const char* data, size_t size);
    void Send(const std::string& data);

protected:
    int64_t m_allRecved = 0;
};

