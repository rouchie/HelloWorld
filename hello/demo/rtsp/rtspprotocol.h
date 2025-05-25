#pragma once

#include "http/httpparser.h"

#include "rtsp-media-copy.h"
#include "rtsp-header-transport.h"

enum rtsp_state_t
{
    RTSP_INIT,
    RTSP_ANNOUNCE,
    RTSP_RECORD,
    RTSP_DESCRIBE,
    RTSP_SETUP,
    RTSP_PLAY,
    RTSP_PAUSE,
    RTSP_TEARDOWN,
    RTSP_OPTIONS,
    RTSP_GET_PARAMETER,
    RTSP_SET_PARAMETER,
};

#define N_MEDIA    8
#define USER_AGENT "www.rouchie.com"

class RtspProtocol : public HttpParser
{
public:
    using Ptr = std::shared_ptr<RtspProtocol>;

public:
    explicit RtspProtocol(const std::string& url);
    ~RtspProtocol() override = default;

protected:
    void OnHttpResponse(RQHttpResponse::Ptr response) override;

protected:
    int RtspInput(const char *data, const size_t size);

protected:
    int Options();
    int Describe();
    int Setup(const std::string& sdp);
    int MediaSetup(int index);
    int Play();

    int OptionsReply(const RQHttpResponse::Ptr& response);
    int DescribeReply(const RQHttpResponse::Ptr& response);
    int SetupReply(const RQHttpResponse::Ptr& response);

    virtual void OnOptions() {}
    virtual void OnDescribe(const std::string& sdp) {}
    virtual void OnSetup() {}
    virtual void OnPlay() {}
    virtual void OnPause() {}
    virtual void OnTeardown() {}

protected:
    int OnSend(const std::string& data);
    virtual int OnSend(const char *data, const size_t size) = 0;

protected:
    std::string m_url;
    std::string m_base_url;

    std::vector<rtsp_media_t> m_media;
    std::vector<rtsp_header_transport_t> m_transport;

    int m_media_count = 0;
    int m_setup_progress = 0;

private:
    int m_state = RTSP_INIT;
    int m_seq = 0;
    std::string m_session;

    bool m_http_parser_need_more = false;
};
