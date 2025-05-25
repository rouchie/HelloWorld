#pragma once

#include "rtspprotocol.h"

#include "utils/UtilsSpdlog.h"

RtspProtocol::RtspProtocol(const std::string &url)
    : HttpParser(true), m_url(url)
{
    m_media.resize(N_MEDIA);
    m_transport.resize(N_MEDIA);
}

void RtspProtocol::OnHttpResponse(RQHttpResponse::Ptr response)
{
    int nRet = 0;
    switch (m_state) {
        case RTSP_OPTIONS: {
            nRet = OptionsReply(response);
        }
        break;
        case RTSP_DESCRIBE: {
            nRet = DescribeReply(response);
        }
        break;
        case RTSP_SETUP: {
            nRet = SetupReply(response);
        }
            break;
        default: break;
    }

    if (nRet < 0) {
        // 协议错误，直接关闭？
    }
}

int RtspProtocol::RtspInput(const char *data, const size_t size)
{
    const char *start = (char *) (data);
    const char *end = (char *) (data + size);

    do {
        if (!m_http_parser_need_more && (*start == '$' || 0)) {
        } else {
            size_t n = end - start;
            const int nRet = InputData(start, &n);
            if (nRet > 0) {
                m_http_parser_need_more = false;
            } else if (nRet == 0) {
                m_http_parser_need_more = true;
            } else {
                // 解析出错了，返回个错误码，然后退出
                return -1;
            }
            start += n;
        }
    } while (start < end && !m_http_parser_need_more);

    return 0;
}

int RtspProtocol::Options()
{
    m_state = RTSP_OPTIONS;

    const std::string options = fmt::format("OPTIONS * RTSP/1.0\r\n"
                                            "CSeq: {}\r\n"
                                            "User-Agent: {}\r\n"
                                            "\r\n", m_seq++, USER_AGENT);

    return OnSend(options);
}

int RtspProtocol::Describe()
{
    m_state = RTSP_DESCRIBE;

    const std::string request = fmt::format("DESCRIBE {} RTSP/1.0\r\n"
                                            "CSeq: {}\r\n"
                                            "Accept: application/sdp\r\n"
                                            "User-Agent: {}\r\n"
                                            "\r\n", m_url, m_seq++, USER_AGENT);
    return OnSend(request);
}

int RtspProtocol::Setup(const std::string &sdp)
{
    m_media_count = rtsp_media_sdp(sdp.c_str(), (int) sdp.size(), m_media.data(), (int) m_media.size());

    if (m_media_count <= 0) {
        return -1;
    }

    for (int i = 0; i < m_media_count; i++) {
        rtsp_header_transport_t& t = m_transport[i];
        t.transport = RTSP_TRANSPORT_RTP_TCP;
        t.interleaved1 = 2 * i;
        t.interleaved2 = t.interleaved1 + 1;
    }

    m_state = RTSP_SETUP;

    return MediaSetup(m_setup_progress++);
}

int RtspProtocol::MediaSetup(int index)
{
    rtsp_media_t& m = m_media[index];
    rtsp_header_transport_t& t = m_transport[index];

    std::string transport;
    std::string session;

    if (!m_session.empty()) {
        session = fmt::format("Session: {}\r\n", m_session);
    }

    if (t.transport == RTSP_TRANSPORT_RTP_TCP) {
        transport = fmt::format("Transport: RTP/AVP/TCP;unicast;interleaved={}-{}\r\n", t.interleaved1, t.interleaved2);
    }

    const std::string request = fmt::format("SETUP {}/{} RTSP/1.0\r\n"
                                            "CSeq: {}\r\n"
                                            "{}"
                                            "{}"
                                            "User-Agent: {}\r\n"
                                            "\r\n", m_base_url, m.uri, m_seq++, transport, session, USER_AGENT);
    return OnSend(request);
}

int RtspProtocol::Play()
{
    m_state = RTSP_PLAY;

    const std::string options = fmt::format("OPTIONS * RTSP/1.0\r\n"
                                            "CSeq: {}\r\n"
                                            "User-Agent: {}\r\n"
                                            "\r\n", m_seq++, USER_AGENT);
    return OnSend(options);
}

int RtspProtocol::OptionsReply(const RQHttpResponse::Ptr &response)
{
    if (response->GetStatusCode() == 200) {
        OnOptions();
        return 0;
    }
    return -1;
}

int RtspProtocol::DescribeReply(const RQHttpResponse::Ptr &response)
{
    if (response->GetStatusCode() == 200) {
        const std::string contentType = response->HeaderPair("Content-Type");
        const std::string contentBase = response->HeaderPair("Content-Base");
        const std::string contentLocation = response->HeaderPair("Content-Location");

        m_session = response->HeaderPair("Session");
        m_base_url = contentBase;

        OnDescribe(response->GetBody());

        return 0;
    }
    return -1;
}

int RtspProtocol::SetupReply(const RQHttpResponse::Ptr& response)
{
    if (m_setup_progress >= m_media_count) {
        // PLAY
        return 0;
    }

    return MediaSetup(m_setup_progress++);
}

int RtspProtocol::OnSend(const std::string &data)
{
    return OnSend(data.data(), data.size());
}
