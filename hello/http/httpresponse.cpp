#include "httpresponse.h"

#include "utils/UtilsSpdlog.h"

RQHttpResponse::RQHttpResponse()
	: m_version(), m_status_code(-1), m_status_phrase()
{
}

RQHttpResponse::RQHttpResponse(const std::string& version, int code, const std::string& phrase)
	: m_version(version), m_status_code(code), m_status_phrase(phrase)
{
}

void RQHttpResponse::SetVersion(const std::string& version)
{
	m_version = version;
}

void RQHttpResponse::SetStatusCode(int code)
{
	m_status_code = code;
}

void RQHttpResponse::SetStatusPhrase(const std::string& phrase)
{
	m_status_phrase = phrase;
}

std::string RQHttpResponse::GetVersion() const
{
	return m_version;
}

int RQHttpResponse::GetStatusCode() const
{
	return m_status_code;
}

std::string RQHttpResponse::GetStatusPhrase() const
{
	return m_status_phrase;
}

void RQHttpResponse::AddHeaderPair(const std::string& name, const std::string& value)
{
	m_header[name] = value;
}

void RQHttpResponse::AddHeaderPair(const std::string& name, int64_t value)
{
	m_header[name] = std::to_string(value);
}

std::string RQHttpResponse::HeaderPair(const std::string& name)
{
	const auto it = m_header.find(name);
	if (it == m_header.end()) {
		return {};
	}
	return it->second;
}

bool RQHttpResponse::FindHeaderPair(const std::string& name, std::string& value)
{
	const auto it = m_header.find(name);
	if (it == m_header.end()) {
		return false;
	}
	value = it->second;
	return true;
}

void RQHttpResponse::SetBody(const std::string& body)
{
	AddHeaderPair("Content-Length", std::to_string(body.size()));
	m_body = body;
}

void RQHttpResponse::SetJsonBody(const std::string& body)
{
	AddHeaderPair("Content-Length", std::to_string(body.size()));
	AddHeaderPair("Content-Type", "application/json");
	m_body = body;
}

std::string RQHttpResponse::GetBody() const
{
	return m_body;
}

std::string RQHttpResponse::ToString()
{
	std::string out = fmt::format("{} {} {}\r\n", m_version, m_status_code, m_status_phrase);

	for (auto& it : m_header) {
		out += fmt::format("{}: {}\r\n", it.first, it.second);
	}

	out += "\r\n";
	out += m_body;

	return out;
}

