#include "rqtcpsession.h"
#include "rqevent.h"

#include "utils/UtilsSpdlog.h"
#include "utils/UtilsCommon.h"

#include "workflow/WFTaskFactory.h"

RQTcpSession::RQTcpSession(int64_t uuid)
	: RQObject(IPC_MODULE_ID_TCP_SESSION), RQBaseSession(uuid)
{
	Connect(IPC_COMMAND_ID_TCP_READ, BIND1(&RQTcpSession::Input));
}

RQTcpSession::~RQTcpSession()
{
	SPDLOG_INFO("DEL Session AllRecved: {}", m_allRecved);
}

int RQTcpSession::Input(RQMsg::Ptr msg)
{
	auto binary = msg->Bin();
	Recved(binary.data(), binary.size());
	return 0;
}

void RQTcpSession::Started()
{
	RQBaseSession::Id(m_mid);
}

void RQTcpSession::Send(const char* data, size_t size)
{
	RQEvent::Inst()->PipeTcpSend(m_mid, UUID(), data, size);
}

void RQTcpSession::Send(const std::string& data)
{
	Send(data.data(), data.size());
}
