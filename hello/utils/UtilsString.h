#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <unordered_map>

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

// 去除字符串前后的空格  
inline std::string Trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\n");
    if (first == std::string::npos) return ""; // 如果没有找到非空格字符，则返回空字符串  

    size_t last = str.find_last_not_of(" \t\n");
    return str.substr(first, (last - first + 1));
}

inline bool StrCaseCmp(const std::string& s0, const std::string& s1)
{
    return strcasecmp(s0.c_str(), s1.c_str()) == 0;
}

inline bool EndsWith(const std::string& str, const std::string& suffix)
{
    if (str.length() >= suffix.length()) {
        return (str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0);
    }
    return false;
}

inline std::vector<std::string> Split(const std::string& str, const char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        token = str.substr(start, end - start);
        tokens.push_back(token);
        start = end + 1;  // Skip past the delimiter  
        end = str.find(delimiter, start);
    }

    // Add the last token  
    token = str.substr(start);
    tokens.push_back(token);

    return tokens;
}

inline std::string Join(const std::vector<std::string>& vec, const std::string& delimiter = " ")
{
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];  // 将每个字符串添加到流中  
        if (i != vec.size() - 1) { // 如果不是最后一个元素，添加分隔符  
            oss << delimiter;
        }
    }
    return oss.str(); // 返回合并后的字符串  
}

struct CaseInsensitiveHash {
    size_t operator()(const std::string& key) const {
        std::string lowerKey;
        lowerKey.reserve(key.size());
        for (char c : key) {
            lowerKey.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
        return std::hash<std::string>()(lowerKey);
    }
};

struct CaseInsensitiveEqual {
    bool operator()(const std::string& lhs, const std::string& rhs) const {
        if (lhs.size() != rhs.size()) return false;
        for (size_t i = 0; i < lhs.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(lhs[i])) != std::tolower(static_cast<unsigned char>(rhs[i]))) {
                return false;
            }
        }
        return true;
    }
};

// 忽略大小写的 unordered_map，默认value类型 std::string
template <typename Value = std::string>
using RQStringMap = std::unordered_map<std::string, Value, CaseInsensitiveHash, CaseInsensitiveEqual>;
