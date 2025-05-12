#pragma once

#include <functional>
#include "spdlog/spdlog.h"

#define BIND0(Func) std::bind(Func, this)
#define BIND1(Func) std::bind(Func, this, std::placeholders::_1)
#define BIND2(Func) std::bind(Func, this, std::placeholders::_1, std::placeholders::_2)
#define BIND3(Func) std::bind(Func, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

#define THROW(ABC, ...) {                                                                \
	std::string e = fmt::format("throw [{}:{}] " ABC, __FILE__, __LINE__, ##__VA_ARGS__);\
	SPDLOG_ERROR("{}", e);                                                               \
	throw std::runtime_error(e);                                                         \
}

class One
{
public:
	One(const std::function<void()> &f0, const std::function<void()> &f1) : m_release(f1) {
		if (f0) f0();
	}

	~One() {
		if (m_release) m_release();
	}

private:
	std::function<void()> m_release;
};

// 参考go语言里的defer关键字
class Defer
{
public:
	explicit Defer(const std::function<void()> &f1) : m_release(f1) {}

	Defer(const std::function<void()> &f0, const std::function<void()> &f1) : m_release(f1) {
		if (f0) f0();
	}

	~Defer() {
		if (m_release) m_release();
	}

private:
	std::function<void()> m_release;
};
