#pragma once

#include "utils/UtilsJson.h"

#include "leopard/rqtcpsession.h"

#include "http/httpparser.h"

class RQHttpSession : public RQTcpSession
{
public:
    RQHttpSession(int64_t uuid);
    ~RQHttpSession();

protected:
    void Recved(const char* data, size_t size) override;

protected:
    virtual void OnHttpRequestError(int nError);
    virtual void OnHttpRequest();

protected:
    virtual void OnHead();
    virtual void OnGet();
    virtual void OnPut();
    virtual void OnPost();
    virtual void OnDelete();

protected:
    void Response(int code, const std::string& message, nlohmann::json body);

protected:
    HttpParser m_req;

protected:
    RQStringMap<std::function<void()>> m_funcs;
};
