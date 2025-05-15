#include "rtspclient.h"

#include "utils/UtilsSpdlog.h"

void RtspClient::OnConnected(int64_t uuid)
{
}

void RtspClient::OnDisconnected(int64_t code)
{
    SPDLOG_INFO("{}:{}", m_bConnected, code);
}

void RtspClient::OnRecved(const char *data, size_t size)
{
}

