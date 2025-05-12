#include "rqtcpclient.h"
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
	SPDLOG_INFO("DEL Session AllRecved[{}] AllSended[{}]", m_allRecved, m_allSended);
	Stop();
}

void RQTcpClient::Start(const std::string& ip, int port, OnClose onclose)
{
	m_ip = ip;
	m_port = port;

	m_close = onclose;

	RQObject::Start();
	RQEvent::Inst()->PipeAddClient(Mid(), ip, port);
}

void RQTcpClient::Stop()
{
	RQEvent::Inst()->PipeDelClient(Mid(), UUID());
}

void RQTcpClient::ReStart()
{
	Stop();
	Start(m_ip, m_port);
}

int RQTcpClient::Input(RQMsg::PTR msg)
{
	auto binary = msg->Binary();

	m_allRecved += binary.size();

	Recved(binary.data(), binary.size());

	return 0;
}

int RQTcpClient::TcpConnected(RQMsg::PTR msg)
{
	UUID(msg->Pam());

	OnConnected(msg->Pam());
	return 0;
}

int RQTcpClient::TcpDisconnected(RQMsg::PTR msg)
{
	OnDisconnected();

	if (m_close) m_close();

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
