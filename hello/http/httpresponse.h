#pragma once

#include "utils/UtilsString.h"

class RQHttpResponse : public std::enable_shared_from_this<RQHttpResponse>
{
public:
    RQHttpResponse();
    RQHttpResponse(const std::string& version, int code, const std::string& phrase);

public:
    void SetVersion(const std::string& version);
    void SetStatusCode(int code);
    void SetStatusPhrase(const std::string& phrase);

public:
    void AddHeaderPair(const std::string& name, const std::string& value);
    void AddHeaderPair(const std::string& name, int64_t value);

public:
    void SetBody(const std::string& body);
    void SetJsonBody(const std::string& body);

public:
    std::string ToString();

protected:
    std::string m_version;
    int m_status_code;
    std::string m_status_phrase;

	RQStringMap<> m_header;
    std::string m_body;
};

