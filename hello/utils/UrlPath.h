#pragma once

#include <map>
#include <vector>
#include <regex>
#include <sstream>

class CUrlPath {
public:
    CUrlPath() = default;
    CUrlPath(const std::string& url);

    const std::string& Api();
    const std::vector<std::string>& Apis();

    const std::string& Path();
    const std::vector<std::string>& Paths();

    std::string Query(const std::string& key);
    const std::map<std::string, std::string>& Querys();

private:
    std::string m_api;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
    std::map<std::string, std::string> m_querys;
    std::vector<std::string> m_paths;
    std::vector<std::string> m_apis;
};

static std::map<std::string, std::string> ParseQueryParams(const std::string& query)
{
    std::map<std::string, std::string> params;

    size_t pos = query.find('?');
    std::string queryString = query.substr(pos + 1);

    std::string key;
    std::string value;
    std::stringstream ss(queryString);

    while (getline(ss, key, '&'))
    {
        size_t equalPos = key.find('=');
        if (equalPos != std::string::npos)
        {
            std::string keyName = key.substr(0, equalPos);
            std::string keyValue = key.substr(equalPos + 1);
            params[keyName] = keyValue;
        }
    }

    return params;
}

static std::vector<std::string> ParsePathParams(const std::string& path)
{
    std::vector<std::string> params;
    std::string param;

    std::stringstream ss(path);

    while (getline(ss, param, '/'))
    {
        if (param.empty()) continue;

        params.push_back(param);
    }

    return params;
}

inline CUrlPath::CUrlPath(const std::string& url) : m_api("/")
{
    size_t protocol_end = url.find("://");
    size_t host_start = 0; // Skip "://"

    if (protocol_end != std::string::npos)
    {
        host_start = protocol_end + 3;
    }

    size_t path_start = url.find('/', host_start);
    size_t port_start = url.find(':', host_start);

    if (path_start == std::string::npos) {
        path_start = host_start;
    }

    size_t query_start = url.find('?', path_start);
    size_t fragment_start = url.find('#', path_start);

    if (query_start == std::string::npos) {
        query_start = url.length();
    }

    if (fragment_start == std::string::npos) {
        fragment_start = url.length();
    }

    auto m = query_start < fragment_start ? query_start : fragment_start;
    m_path = url.substr(path_start, m - path_start);

    if (query_start < fragment_start) {
        m_query = url.substr(query_start + 1, fragment_start - query_start - 1);
    }

    if (fragment_start < url.length()) {
        m_fragment = url.substr(fragment_start + 1);
    }

    m_querys = ParseQueryParams(m_query);
    m_paths = ParsePathParams(m_path);
    if (m_paths.size())
    {
        m_api += m_paths[0];
    }

    std::string p;
    for (auto& path : m_paths)
    {
        p += "/" + path;
        m_apis.emplace_back(p);
    }
}

inline const std::string& CUrlPath::Api()
{
	return m_api;
}

inline const std::vector<std::string>& CUrlPath::Apis()
{
	return m_apis;
}

inline const std::string& CUrlPath::Path()
{
	return m_path;
}

inline const std::vector<std::string>& CUrlPath::Paths()
{
	return m_paths;
}

inline std::string CUrlPath::Query(const std::string& key)
{
    auto it = m_querys.find(key);
    return it != m_querys.end() ? it->second : std::string();
}

inline const std::map<std::string, std::string>& CUrlPath::Querys()
{
	return m_querys;
}
