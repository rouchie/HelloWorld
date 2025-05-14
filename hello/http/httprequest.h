#pragma once

#include "utils/UtilsString.h"

class RQHttpRequest : public std::enable_shared_from_this<RQHttpRequest>
{
public:
    RQHttpRequest();
    RQHttpRequest(const std::string& method, const std::string& uri, const std::string& version);

public:
    void SetMethod(const std::string& method);
    void SetUri(const std::string& uri);
    void SetVersion(const std::string& version);

public:
    void AddHeaderPair(const std::string& name, const std::string& value);

public:
    void SetBody(const std::string& body);
    void SetJsonBody(const std::string& body);

public:
    std::string ToString();

protected:
    std::string m_method;
    std::string m_uri;
    std::string m_version;

	RQStringMap<> m_header;
    std::string m_body;
};
