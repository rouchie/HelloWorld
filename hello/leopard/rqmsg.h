#pragma once

#include <iostream>

#include "rqdefine.h"
#include "spdlog/spdlog.h"

class RQMsg : public std::enable_shared_from_this<RQMsg>
{
public:
    using PTR = std::shared_ptr<RQMsg>;

public:
	static PTR Builder(cid_t command)
	{
		auto msg = std::make_shared<RQMsg>(0, command);
		return msg;
	}

	static PTR Builder(mid_t sender, cid_t command)
	{
		auto msg = std::make_shared<RQMsg>(sender, 0, command, 0);
		return msg;
	}

	static PTR Builder(mid_t sender, cid_t command, int64_t number)
	{
		auto msg = std::make_shared<RQMsg>(sender, 0, command, number);
		return msg;
	}

	static PTR Builder(mid_t sender, cid_t command, int64_t number, const std::string& message)
	{
		auto msg = std::make_shared<RQMsg>(sender, 0, command, number, message);
		return msg;
	}

public:
    RQMsg(mid_t recver, cid_t command);
    RQMsg(mid_t recver, cid_t command, int64_t number);
    RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number);
    RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number, int64_t count);
    RQMsg(mid_t recver, cid_t command, const std::string& message);
    RQMsg(mid_t sender, mid_t recver, cid_t command, const std::string& message);
    RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number, const std::string& message);
    RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number, const std::string& message, const std::string& binary);
    RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number, int64_t count, const std::string& message, const std::string& binary);

public:
	PTR Clone();

public:
	PTR Snd(mid_t sender);
	PTR Rcv(mid_t recver);
	PTR Cmd(cid_t command);
	PTR Num(int64_t number);
	PTR Pam(int64_t param);
	PTR Msg(const std::string& message);
	PTR Bin(const std::string& binary);

public:
	mid_t Snd() const { return m_sender; }
	mid_t Rcv() const { return m_recver; }
	mid_t Cmd() const { return m_command; }
	int64_t Num() const { return m_number; }
	int64_t Pam() const { return m_param; }
	int64_t Cnt() const { return m_param; }
	const std::string& Msg() { return m_message; }
	const std::string& Bin() { return m_binary; }

public:
	mid_t Sender() const { return m_sender; }
	mid_t Recver() const { return m_recver; }
	cid_t Command() const { return m_command; }
	int64_t Number() const { return m_number; }
	int64_t& Count() { return m_param; }
	const std::string& Message() const { return m_message; }
	const std::string& Binary() const { return m_binary; }

private:
	mid_t m_recver = 0;
	mid_t m_sender = 0;
	cid_t m_command = 0;
	int64_t m_number = 0;
	int64_t m_param = 0;
	std::string m_message;
	std::string m_binary;
};

template <>
struct fmt::formatter<RQMsg> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    auto format(const RQMsg& p, format_context& ctx) const {
		std::string s;
		if (p.Sender() != 0) {
			s += fmt::format("Sender[{}] ", p.Sender());
		}
		if (p.Recver() != 0) {
			s += fmt::format("Recver[{}] ", p.Recver());
		}

		s += fmt::format("Command[{}] ", p.Command());

		if (p.Number() != 0) {
			s += fmt::format("Number[{}] ", p.Number());
		}
		if (const_cast<RQMsg&>(p).Count()) {
			s += fmt::format("Count[{}] ", const_cast<RQMsg&>(p).Count());
		}
		if (p.Message().size()) {
			s += fmt::format("Message[{}] ", p.Message());
		}
		if (p.Binary().size()) {
			s += fmt::format("Binary[{}] ", p.Binary().size());
		}
        return fmt::format_to(ctx.out(), "{}", s);
    }
};

template <>
struct fmt::formatter<std::shared_ptr<RQMsg>> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	auto format(const std::shared_ptr<RQMsg>& ptr, format_context& ctx) const {
		if (ptr) {
			return fmt::format_to(ctx.out(), "{}", *ptr);
		}
		else {
			// 处理空指针情况
			return fmt::format_to(ctx.out(), "[nullptr RQMsg]");
		}
	}
};

#define MSG(...) std::make_shared<RQMsg>(__VA_ARGS__)

