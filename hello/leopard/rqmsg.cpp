#include "rqmsg.h"

RQMsg::RQMsg(mid_t recver, cid_t command)
	: m_recver(recver), m_command(command)
{
}

RQMsg::RQMsg(mid_t recver, cid_t command, int64_t number)
	: m_recver(recver), m_command(command), m_number(number)
{
}

RQMsg::RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number)
	: m_sender(sender), m_recver(recver), m_command(command), m_number(number)
{
}

RQMsg::RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number, int64_t count)
	: m_sender(sender), m_recver(recver), m_command(command), m_number(number), m_param(count)
{
}

RQMsg::RQMsg(mid_t recver, cid_t command, const std::string& message)
	: m_recver(recver), m_command(command), m_message(message)
{
}

RQMsg::RQMsg(mid_t sender, mid_t recver, cid_t command, const std::string& message)
	: m_sender(sender), m_recver(recver), m_command(command), m_message(message)
{
}

RQMsg::RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number, const std::string& message)
	: m_sender(sender), m_recver(recver), m_command(command), m_number(number), m_message(message)
{
}

RQMsg::RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number, const std::string& message, const std::string& binary)
	: m_sender(sender), m_recver(recver), m_command(command), m_number(number), m_message(message), m_binary(binary)
{
}

RQMsg::RQMsg(mid_t sender, mid_t recver, cid_t command, int64_t number, int64_t count, const std::string& message, const std::string& binary)
	: m_sender(sender), m_recver(recver), m_command(command), m_number(number), m_param(count), m_message(message), m_binary(binary)
{
}

RQMsg::PTR RQMsg::Clone()
{
	return nullptr;
}

RQMsg::PTR RQMsg::Snd(mid_t sender)
{
	m_sender = sender;
	return this->shared_from_this();
}

RQMsg::PTR RQMsg::Rcv(mid_t recver)
{
	m_recver = recver;
	return this->shared_from_this();
}

RQMsg::PTR RQMsg::Cmd(cid_t command)
{
	m_command = command;
	return this->shared_from_this();
}

RQMsg::PTR RQMsg::Num(int64_t number)
{
	m_number = number;
	return this->shared_from_this();
}

RQMsg::PTR RQMsg::Pam(int64_t param)
{
	m_param = param;
	return this->shared_from_this();
}

RQMsg::PTR RQMsg::Msg(const std::string& message)
{
	m_message = message;
	return this->shared_from_this();
}

RQMsg::PTR RQMsg::Bin(const std::string& binary)
{
	m_binary = binary;
	return this->shared_from_this();
}
