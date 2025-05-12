#pragma once

#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h> // Windows 头文件
#else
#include <arpa/inet.h> // Linux 头文件
#include <unistd.h>
#endif

/* 网络相关的一些函数
**
*/

inline int GetAllLocalIP(std::vector<std::string>& ips)
{
    int nRet = -1;
    char sHost[256];

    gethostname(sHost, sizeof(sHost));

    // 获得主机名称下的网络信息
    hostent* pHost = ::gethostbyname(sHost); // 返回指定主机名的包含主机名字和地址信息的hostent结构的指针
    if (!pHost) {
        return nRet;
    }

    // 处理获得的网络信息
    int nCount = 0;
    std::string strTmp;
    char szIPTemp[127] = { 0 };
    for (int i = 0; pHost->h_addr_list[i]; ++i) {
        struct in_addr* pAddr = (struct in_addr*)pHost->h_addr_list[i];
        if (inet_ntop(pHost->h_addrtype, (void*)pAddr, szIPTemp, 64) != NULL) {
            if (strcmp(szIPTemp, "127.0.0.1") != NULL) {
                ips.push_back(szIPTemp);
            }
        }
    }

    return 0;
}
