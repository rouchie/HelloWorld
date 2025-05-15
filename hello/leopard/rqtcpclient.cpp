#include "rqtcpclient.h"

#include <utility>
#include "rqevent.h"

#include "utils/UtilsSpdlog.h"
#include "utils/UtilsCommon.h"

#include "workflow/WFTaskFactory.h"

RQTcpClient::RQTcpClient()
	: RQObject(IPC_MODULE_ID_TCP_CLIENT), RQBaseSession()
{
	Connect(IPC_COMMAND_ID_TCP_READ, BIND1(&RQTcpClient::Input));
	Connect(IPC_COMMAND_ID_TCP_CONNECT, BIND1(&RQTcpClient::TcpConnected));
	Connect(IPC_COMMAND_ID_TCP_DISCONNECT, BIND1(&RQTcpClient::TcpDisconnected));
}

RQTcpClient::~RQTcpClient()
{
	SPDLOG_INFO("DEL Session[{:#X}:{:#X}] AllRecved[{}] AllSended[{}]", Mid(), UUID(), m_allRecved, m_allSended);
	Stop();
}

void RQTcpClient::Start(const std::string& ip, int port, OnClose onclose)
{
	m_ip = ip;
	m_port = port;

	m_close = std::move(onclose);

	RQObject::Start();
	RQEvent::Inst()->PipeAddClient(Mid(), ip, port);
}

void RQTcpClient::Stop() const
{
	RQEvent::Inst()->PipeDelClient(Mid(), UUID());
}

void RQTcpClient::ReStart()
{
	Stop();
	Start(m_ip, m_port);
}

int RQTcpClient::Input(const RQMsg::Ptr& msg)
{
	auto binary = msg->Bin();

	m_allRecved += binary.size();

	OnRecved(binary.data(), binary.size());

	return 0;
}

int RQTcpClient::TcpConnected(const RQMsg::Ptr& msg)
{
	UUID(msg->Pam());

	OnConnected(msg->Pam());
	return 0;
}

int RQTcpClient::TcpDisconnected(const RQMsg::Ptr& msg)
{
	OnDisconnected(msg->Pam());

	return 0;
}

void RQTcpClient::Send(const char* data, size_t size)
{
	m_allSended += size;
	RQEvent::Inst()->PipeTcpSend(m_mid, UUID(), data, size);
}

void RQTcpClient::Send(const std::string& data)
{
	Send(data.data(), data.size());
}
