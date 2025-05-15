#pragma once

#include <iostream>

#include "rqdefine.h"
#include "spdlog/spdlog.h"

class RQMsg : public std::enable_shared_from_this<RQMsg>
{
public:
    using Ptr = std::shared_ptr<RQMsg>;

public:
	static Ptr Builder(cid_t command)
	{
		auto msg = std::make_shared<RQMsg>(command);
		return msg;
	}

	static Ptr Builder(mid_t sender, cid_t command)
	{
		auto msg = std::make_shared<RQMsg>(sender,command, 0);
		return msg;
	}

	static Ptr Builder(mid_t sender, cid_t command, int64_t number)
	{
		auto msg = std::make_shared<RQMsg>(sender, command, number);
		return msg;
	}

	static Ptr Builder(mid_t sender, cid_t command, int64_t number, const std::string& message)
	{
		auto msg = std::make_shared<RQMsg>(sender, command, number, message);
		return msg;
	}

public:
    explicit RQMsg(const RQMsg::Ptr& msg);

    explicit RQMsg(cid_t command);
    RQMsg(cid_t command, int64_t number);
    RQMsg(mid_t sender, cid_t command, int64_t number);
    RQMsg(mid_t sender, cid_t command, int64_t number, int64_t count);
    RQMsg(cid_t command, const std::string& message);
    RQMsg(mid_t sender, cid_t command, const std::string& message);
    RQMsg(mid_t sender, cid_t command, int64_t number, const std::string& message);
    RQMsg(mid_t sender, cid_t command, int64_t number, const std::string& message, const std::string& binary);
    RQMsg(mid_t sender, cid_t command, int64_t number, int64_t count, const std::string& message, const std::string& binary);

public:
	virtual Ptr Clone() const;

public:
	Ptr Snd(mid_t sender);
	Ptr Cmd(cid_t command);
	Ptr Num(int64_t number);
	Ptr Pam(int64_t param);
	Ptr Msg(const std::string& message);
	Ptr Bin(const std::string& binary);

public:
	mid_t Snd() const { return m_sender; }
	cid_t Cmd() const { return m_command; }
	int64_t Num() const { return m_number; }
	int64_t Pam() const { return m_param; }
	const std::string& Msg() const { return m_message; }
	const std::string& Bin() const { return m_binary; }

private:
	mid_t m_sender = 0;
	cid_t m_command = 0;
	int64_t m_number = 0;
	int64_t m_param = 0;
	std::string m_message;
	std::string m_binary;
};

template <>
struct fmt::formatter<RQMsg> {
    static constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    static auto format(const RQMsg& p, format_context& ctx) {
		std::string s;
		if (p.Snd() != 0) {
			s += fmt::format("Sender[{}] ", p.Snd());
		}

		s += fmt::format("Command[{}] ", p.Cmd());

		if (p.Num() != 0) {
			s += fmt::format("Number[{}] ", p.Num());
		}
		if (const_cast<RQMsg&>(p).Pam()) {
			s += fmt::format("Count[{}] ", const_cast<RQMsg&>(p).Pam());
		}
		if (!p.Msg().empty()) {
			s += fmt::format("Message[{}] ", p.Msg());
		}
		if (!p.Bin().empty()) {
			s += fmt::format("Binary[{}] ", p.Bin().size());
		}
        return fmt::format_to(ctx.out(), "{}", s);
    }
};

template <>
struct fmt::formatter<std::shared_ptr<RQMsg>> {
	static constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	static auto format(const std::shared_ptr<RQMsg>& ptr, format_context& ctx) {
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

