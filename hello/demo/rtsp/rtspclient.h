#pragma once

#include "leopard/rqtcpclient.h"

class RtspProtocol
{
public:
    int Input(const char *data, size_t size)
    {
        const uint8_t* start = (uint8_t*) (data);
        const uint8_t* end = (uint8_t*) (data + size);

        do {
            if (!m_needMore && (*start == '$' || 0 != m_rtp->state)) {
                start = rtp_over_rtsp(m_rtp.get(), start, end);
            }
            else {
                // 上次还没解析完成
                size_t size = end - start;
                int nRet = m_httpParser.InputData((char*)start, &size);
                if (nRet > 0) {
                    OnProtocol();
                    m_needMore = false;
                }
                else if (nRet == 0) {
                    m_needMore = true;
                }
                else {
                    // 解析出错了
                }
                start += size;
            }
        } while (start < end && !m_needMore);
    }
};

class RtspClient final : public RQTcpClient, public RtspProtocol
{
public:
    RtspClient() = default;

    ~RtspClient() override = default;

protected:
    void OnConnected(int64_t uuid) override;

    void OnDisconnected(int64_t code) override;

    void OnRecved(const char *data, size_t size) override;

private:
    bool m_bConnected = false;
};
