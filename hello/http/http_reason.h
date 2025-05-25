#pragma once

#include <iostream>
#include <unordered_map>

inline std::string HttpReasonPhrase(int code)
{
    static const std::unordered_map<int, std::string> httpReasons = {
        {200, "OK"},
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {403, "Forbidden"},
        {404, "Not Found"},

        // RTSP 错误码
        {451, "Parameter Not Understood"}, // 451
        {452, "Conference Not Found"}, // 452
        {453, "Not Enough Bandwidth"}, // 453
        {454, "Session Not Found"}, // 454
        {455, "Method Not Valid in This State"}, // 455
        {456, "Header Field Not Valid for Resource"}, // 456
        {457, "Invalid Range"}, // 457
        {458, "Parameter Is Read-Only"}, // 458
        {459, "Aggregate Operation Not Allowed"}, // 459
        {460, "Only Aggregate Operation Allowed"}, // 460
        {461, "Unsupported Transport"}, // 461
        {462, "Destination Unreachable"}, // 462
        {500, "Internal Server Error"},
        {503, "Service Unavailable"},
        {505, "RTSP Version Not Supported"},
        {551, "Option not supported"},
    };

    const auto it = httpReasons.find(code);
    if (it != httpReasons.end()) {
        return it->second;
    }
    return "Unknown";
}
