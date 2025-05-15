#include "utils/UtilsSemaphore.h"
#include "utils/UrlPath.h"

#include "leopard/rqcoremanager.h"
#include "leopard/rqtcpclient.h"
#include "http/httprequest.h"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#endif

// const std::string uri = "rtsp://gwm-000-cn-0448.bcloud365.net:9012/rec/bfe1011fc564c98c/Mnx8YmVjMDIxYzIwZTI5ZWYyNWI2YTM5ZGJjYjgxMWQyZmJ8fGJmZTEwMTFmYzU2NGM5OGN8fDlmMGYwMzkzNjAzNzcxMWZiNGMxYWJmM2E0NWM07NDVjMDhjMzA2MTlhMmEzMTM5NTNhOThjMTAwNTJiOTU3YWR8fHJ0c3B8fDE3NDcyMDQ4NTUyMTZ8fDE4NDQ2NzQ0MDcxNTYyMDY3OTY4fHxHV00%3D.89cd191948f7d622e33351ad2ab03228.dat";
const std::string uri =
        "rtsp://10.10.22.116:9012/rec/bfe1011fc564c98c/Mnx8YmVjMDIxYzIwZTI5ZWYyNWI2YTM5ZGJjYjgxMWQyZmJ8fGJmZTEwMTFmYzU2NGM5OGN8fGI3YjI4MWViMWVjZjg5MzBiOTQ5OTgyZGI4YTM41ZTE2YTJjOGE0MGJiZmE2ZjJhMWM3NTUwMDgxNWFlMDE0NGN8fHJ0c3B8fDE3NDcyMDk1MjE1OTd8fDE4NDQ2NzQ0MDcxNTYyMDY3OTY4fHxHV00%3D.33a4b46eb6b6f843c53f5412320f1eb9.dat";

class RtspClient final : public RQTcpClient
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

void RtspClient::OnConnected(int64_t uuid)
{
    m_bConnected = true;

    RQHttpRequest req("PLAY", uri, "RTSP/1.0");
    req.AddHeaderPair("Host", "gwm.jftech.pro:9012");
    req.AddHeaderPair("Speed", "4");
    req.AddHeaderPair("CSeq", "0");
    req.AddHeaderPair("Content-Length", "0");

    Send(req.ToString()); {
        RQHttpRequest req("SETUP", uri, "RTSP/1.0");
        req.AddHeaderPair("Host", "gwm.jftech.pro:9012");
        req.AddHeaderPair("CSeq", "123");
        req.AddHeaderPair("Content-Length", "0");

        Send(req.ToString());
    }
}

void RtspClient::OnDisconnected(int64_t code)
{
    SPDLOG_INFO("{}:{}", m_bConnected, code);
    if (m_bConnected == false) {
        m_close();
    }
}

void RtspClient::OnRecved(const char *data, size_t size)
{
    static int i = 0;
    if (i == 0) {
        RQHttpRequest req("PLAY", uri, "RTSP/1.0");
        req.AddHeaderPair("Host", "gwm.jftech.pro:9012");
        req.AddHeaderPair("CSeq", "124");
        req.AddHeaderPair("Content-Length", "0");

        Send(req.ToString());
    }

    i++;
}

int main(int ac, char **av)
{
    SPDLOG_INFO("Start");

    RQCoreManager::Inst()->Start(IPC_SERVER_ID_HELLO);

    {
        RQTcpClient::PTR rtsp = std::make_shared<RtspClient>();
        rtsp->Start("127.0.0.1", 9012, [rtsp]() mutable {
        });
    }

    CSemaphore semm;
    semm.Wait();

    return 0;
}
