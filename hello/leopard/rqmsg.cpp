#include "rqmsg.h"

RQMsg::RQMsg(const RQMsg::Ptr& msg)
{
	if (msg) {
		m_sender = msg->m_sender;
		m_command = msg->m_command;
		m_number = msg->m_number;
		m_param = msg->m_param;
		m_message = msg->m_message;
		m_binary = msg->m_binary;
	}
}

RQMsg::RQMsg(cid_t command)
	: m_command(command)
{
}

RQMsg::RQMsg(cid_t command, int64_t number)
	: m_command(command), m_number(number)
{
}

RQMsg::RQMsg(mid_t sender, cid_t command, int64_t number)
	: m_sender(sender), m_command(command), m_number(number)
{
}

RQMsg::RQMsg(mid_t sender, cid_t command, int64_t number, int64_t count)
	: m_sender(sender), m_command(command), m_number(number), m_param(count)
{
}

RQMsg::RQMsg(cid_t command, const std::string& message)
	: m_command(command), m_message(message)
{
}

RQMsg::RQMsg(mid_t sender, cid_t command, const std::string& message)
	: m_sender(sender), m_command(command), m_message(message)
{
}

RQMsg::RQMsg(mid_t sender, cid_t command, int64_t number, const std::string& message)
	: m_sender(sender), m_command(command), m_number(number), m_message(message)
{
}

RQMsg::RQMsg(mid_t sender, cid_t command, int64_t number, const std::string& message, const std::string& binary)
	: m_sender(sender), m_command(command), m_number(number), m_message(message), m_binary(binary)
{
}

RQMsg::RQMsg(mid_t sender, cid_t command, int64_t number, int64_t count, const std::string& message, const std::string& binary)
	: m_sender(sender), m_command(command), m_number(number), m_param(count), m_message(message), m_binary(binary)
{
}

RQMsg::Ptr RQMsg::Clone() const
{
	return Builder(m_sender, m_command, m_number, m_message)->Pam(m_param)->Bin(m_binary);
}

RQMsg::Ptr RQMsg::Snd(mid_t sender)
{
	m_sender = sender;
	return this->shared_from_this();
}

RQMsg::Ptr RQMsg::Cmd(cid_t command)
{
	m_command = command;
	return this->shared_from_this();
}

RQMsg::Ptr RQMsg::Num(int64_t number)
{
	m_number = number;
	return this->shared_from_this();
}

RQMsg::Ptr RQMsg::Pam(int64_t param)
{
	m_param = param;
	return this->shared_from_this();
}

RQMsg::Ptr RQMsg::Msg(const std::string& message)
{
	m_message = message;
	return this->shared_from_this();
}

RQMsg::Ptr RQMsg::Bin(const std::string& binary)
{
	m_binary = binary;
	return this->shared_from_this();
}
