#include "httprequest.h"

#include "utils/UtilsSpdlog.h"

RQHttpRequest::RQHttpRequest()
	: m_method("GET"), m_uri("/"), m_version("HTTP/1.1")
{
}

RQHttpRequest::RQHttpRequest(const std::string& method, const std::string& uri, const std::string& version)
	: m_method(method), m_uri(uri), m_version(version)
{
}

void RQHttpRequest::SetMethod(const std::string& method)
{
	m_method = method;
}

void RQHttpRequest::SetUri(const std::string& uri)
{
	m_uri = uri;
}

void RQHttpRequest::SetVersion(const std::string& version)
{
	m_version = version;
}

void RQHttpRequest::AddHeaderPair(const std::string& name, const std::string& value)
{
	m_header[name] = value;
}

void RQHttpRequest::SetBody(const std::string& body)
{
	m_body = body;
}

void RQHttpRequest::SetJsonBody(const std::string& body)
{
	AddHeaderPair("Content-Length", std::to_string(body.size()));
	AddHeaderPair("Content-Type", "application/json");
	m_body = body;
}

std::string RQHttpRequest::ToString()
{
	std::string out = fmt::format("{} {} {}\r\n", m_method, m_uri, m_version);

	for (auto& it : m_header) {
		out += fmt::format("{}: {}\r\n", it.first, it.second);
	}

	out += "\r\n";
	out += m_body;

	return out;
}
