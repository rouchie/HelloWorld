#include "utils/UtilsSemaphore.h"
#include "utils/UrlPath.h"

#include "leopard/rqcoremanager.h"
#include "leopard/rqtcp.h"

#include "hello/hellosession.h"
#include "rtsp/rtspclient.h"

#include "http/httpparser.h"

#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#endif

int main(int ac, char **av)
{
    SPDLOG_INFO("Start");

    RQCoreManager::Inst()->Start(IPC_SERVER_ID_HELLO);

    {
        const std::string uri = "rtsp://127.0.0.1:40554/myapp/test.mp4";

        auto rtsp = std::make_shared<RtspClient>(uri, "admin", "admin");
        rtsp->Start(uri, [rtsp](int64_t code) mutable {
            rtsp.reset();
        });
    }

    int nRet = 0;

    nRet = RQTcpServer<HelloSession>(50000);
    if (nRet < 0) {}

    CSemaphore semm;
    semm.Wait();

    return 0;
}
