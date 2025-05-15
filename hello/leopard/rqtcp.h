#pragma once

#include "rqobject.h"
#include "rqcoremanager.h"

#include "utils/UtilsSemaphore.h"

template <typename SESSION>
int RQTcpServer(int port)
{
    auto f = [](int64_t uuid) -> RQTcpSession::PTR {
        RQTcpSession::PTR object = std::static_pointer_cast<RQTcpSession>(RQCreateObject<SESSION>(uuid));
        return object;
    };

    CSemaphore sem;
    int result;

    mid_t mid = IPC_MODULE_ID_TCP_SERVER;
    RQCoreManager::Inst()->AddModule(mid, [&result, &sem](RQMsg::Ptr msg) -> int {
        result = msg->Num();
        sem.Post();
        return 0;
    });

	RQEvent::Inst()->PipeAddListen(mid, port, f);

    sem.Wait();

    return result;
}
