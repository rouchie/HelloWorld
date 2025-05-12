#pragma once

#include "nlohmann/json.hpp"
#include "UtilsSpdlog.h"

inline bool IsJson(const std::string& json)
{
	try {
		auto js = nlohmann::json::parse(json);
	}
	catch (...) {
		return false;
	}

	return true;
}

inline nlohmann::json ParseJson(const std::string& json)
{
	nlohmann::json j;

	if (json.empty()) {
		return j;
	}

	try {
		j = nlohmann::json::parse(json);
	}
	catch (const std::exception& e) {
		SPDLOG_WARN("Parse Json Failed With Error[{}]", e.what());
	}

	return j;
}

template <typename T>
struct ReturnType
{
	using Type = T;
};

template <>
struct ReturnType<const char*>
{
	using Type = std::string;
};

template <typename T>
inline typename ReturnType<T>::Type get(nlohmann::json json, const std::string& name, T tDefVal)
{
	typename ReturnType<T>::Type t = tDefVal;

	if (json[name].is_null()) {
		return t;
	}

	try {
		t = json[name].get<typename ReturnType<T>::Type>();
	}
	catch (const std::exception& e) {
		SPDLOG_WARN("Get Json[{}] Failed With Error[{}]", name, e.what());
	}

	return t;
}

template <typename T>
inline typename ReturnType<T>::Type get(nlohmann::json json, const std::string& first, const std::string& second, T tDefVal)
{
	if (json[first].is_null() || !json[first].is_object()) {
		return tDefVal;
	}

	return get(json[first], second, tDefVal);
}



