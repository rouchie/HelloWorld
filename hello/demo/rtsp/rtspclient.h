#pragma once

#include "rtspprotocol.h"

#include "leopard/rqtcpclient.h"

#define RTSP_BAD_URL       40001
#define RTSP_PARSER_FAILED 40002

class RtspClient final : public RQTcpClient, public RtspProtocol
{
public:
    using Ptr = std::shared_ptr<RtspClient>;

public:
    RtspClient(const std::string& url, const std::string& name, const std::string& pwd);
    ~RtspClient() override = default;

public:
    void Start(const std::string& url, OnClose close);

protected:
    int OnSend(const char *data, const size_t size) override;

    void OnOptions() override;
    void OnDescribe(const std::string& sdp) override;

protected:
    void OnConnected(int64_t uuid) override;
    void OnDisconnected(int64_t code) override;
    void OnRecved(const char *data, size_t size) override;

private:
    bool m_bConnected = false;
};
