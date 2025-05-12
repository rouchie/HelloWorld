#pragma once

#include "utils/UtilsString.h"

#include "workflow/http_parser.h"

class HttpParser
{
public:
    HttpParser(bool is_resp = false);

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

private:
    std::shared_ptr<http_parser_t> m_ptrParser;
};
