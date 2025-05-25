#pragma once

#include "utils/UtilsString.h"
#include "http_parser.h"
#include "httpresponse.h"

class HttpParser
{
public:
    using Ptr = std::shared_ptr<HttpParser>;

public:
    explicit HttpParser(bool is_resp = false);
    virtual ~HttpParser() = default;

public:
    /**
     * @brief 解析http协议
     * @param data
     * @param size
     * @return 0:数据不完整 >0:解析成功 <0:解析失败
     */
    int InputData(const char *data, size_t *size) const;

public:
    virtual void OnHttpResponse(RQHttpResponse::Ptr response);

public:
    std::string m_header_key;
    bool m_is_complete = false;

// inner

public:
    RQHttpResponse::Ptr m_response;

private:
    std::shared_ptr<http_parser> m_ptr_parser;
    bool m_is_resp = false;
};

#if 0
#include "workflow/http_parser.h"

class HttpParser
{
public:
    explicit HttpParser(bool is_resp = false);

public:
    /**
     * @brief 解析http协议
     * @param data 
     * @param size 
     * @return 0: 数据不完整 >0: 解析成功 <0: 解析失败
     */
    int InputData(const char* data, size_t* size);

public:
    std::string m_method;
    std::string m_uri;
    std::string m_path;
    std::string m_version;
    std::string m_body;
	RQStringMap<> m_query;
	RQStringMap<> m_header;

    std::string m_code;
    std::string m_phrase;
private:
    std::shared_ptr<http_parser_t> m_ptrParser;
    bool m_is_resp = false;
};
#endif
