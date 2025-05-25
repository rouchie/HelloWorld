#include "rtspclient.h"

#include "utils/UtilsSpdlog.h"
#include "utils/UtilsString.h"

#include <regex>

// 返回是否成功解析，host 和 port 通过引用返回
static bool ExtractRtspInfo(const std::string& url, std::string& host, int& port)
{
    // 默认值（解析失败时可能保持不变）
    port = 554;
    host.clear();

    // 正则表达式匹配 RTSP URL
    std::regex rtsp_regex(R"(^rtsp://([^:/?#]+)(?::(\d+))?(?:/.*)?$)");
    std::smatch match;

    if (!std::regex_match(url, match, rtsp_regex)) {
        return false; // 格式不匹配，解析失败
    }

    if (match.size() < 2) {
        return false; // 无主机部分，解析失败
    }

    host = match[1].str(); // 提取主机（IP 或域名）

    // 如果有端口号（第3个分组）
    if (match.size() >= 3 && match[2].matched) {
        try {
            port = stoi(match[2].str());
        } catch (...) {
            return false; // 端口号转换失败（非数字）
        }
    }

    return true; // 解析成功
}

RtspClient::RtspClient(const std::string& url, const std::string& name, const std::string& pwd)
    : RtspProtocol(url)
{
}

void RtspClient::Start(const std::string& url, OnClose close)
{
    std::string ip;
    int port;
    if (!ExtractRtspInfo(m_url, ip, port)) {
        // 错误的url, 直接调用close
        close(RTSP_BAD_URL);
    }
    RQTcpClient::Start(ip, port, close);
}

int RtspClient::OnSend(const char *data, const size_t size)
{
    Send(data, size);
    return 0;
}

void RtspClient::OnOptions()
{
}

void RtspClient::OnDescribe(const std::string& sdp)
{
    Setup(sdp);
}

void RtspClient::OnConnected(int64_t uuid)
{
    Describe();
}

void RtspClient::OnDisconnected(int64_t code)
{
    SPDLOG_INFO("{}:{}", m_bConnected, code);
    m_close(code);
}

void RtspClient::OnRecved(const char *data, size_t size)
{
    const int n = RtspInput(data, size);
    if (n < 0) {
        // 解析失败，直接close
        m_close(RTSP_PARSER_FAILED);
    }
}

