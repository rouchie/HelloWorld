#pragma once

#include <string>
#include <openssl/evp.h>
#include <vector>

inline bool Base64Encode(const std::vector<uint8_t>& data, std::string& base64)
{
	size_t nBase64Len = (data.size() + 2) / 3 * 4;
	if (nBase64Len == 0) {
		return false;
	}

	base64.resize(nBase64Len);
	EVP_EncodeBlock((unsigned char*)base64.data(), data.data(), data.size());

    return true;
}

inline std::string Base64Encode(const std::string& data)
{
    std::string base64;
    bool r = Base64Encode(std::vector<uint8_t>(data.begin(), data.end()), base64);
    if (r) {
        return base64;
    }
    return std::string();
}

inline bool Base64Decode(const std::string& base64, std::vector<uint8_t>& data)
{
    size_t nBase64Len = base64.size();
    if (nBase64Len % 4 != 0)
    {
        return false;
    }

    const size_t nDataLen = (nBase64Len / 4) * 3;

    data.resize(nDataLen);

    int nRet = EVP_DecodeBlock((unsigned char*)data.data(), (const unsigned char*)base64.data(), base64.size());
    if (nRet <= 0) {
        return false;
    }

    while (base64.at(--nBase64Len) == '=') {
        nRet--;
        if (base64.size() > nBase64Len + 2) {
            return false;
        }
    }

    data.resize(nRet);

    return true;
}

inline std::string Base64Decode(const std::string& base64)
{
    std::vector<uint8_t> v;
    bool r = Base64Decode(base64, v);
    if (r) {
        return std::string(reinterpret_cast<const char*>(v.data()), v.size());
    }
    return std::string();
}