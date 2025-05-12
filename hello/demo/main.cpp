#include "utils/UtilsSemaphore.h"
#include "utils/UrlPath.h"

#include "leopard/rqcoremanager.h"
#include "leopard/rqtcpclient.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

class RtspClient final : public RQTcpClient
{
public:
    RtspClient() = default;
    ~RtspClient() override = default;

protected:
    void OnConnected(int64_t uuid) override;
    void OnDisconnected() override;
    void Recved(const char *data, size_t size) override;
};

void RtspClient::OnConnected(int64_t uuid)
{
}

void RtspClient::OnDisconnected()
{
}

void RtspClient::Recved(const char *data, size_t size)
{
}

int main(int ac, char **av)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return -1;
    }

    SPDLOG_INFO("Start");

    RQCoreManager::Inst()->Start(IPC_SERVER_ID_HELLO);

    const RQTcpClient::PTR rtsp = std::make_shared<RtspClient>();
    rtsp->Start("127.0.0.1", 554);

    CSemaphore semm;
    semm.Wait();

    return 0;
}


