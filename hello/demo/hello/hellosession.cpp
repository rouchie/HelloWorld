#include "hellosession.h"

#include "utils/UtilsSpdlog.h"

HelloSession::HelloSession(int64_t uuid)
	: RQTcpSession(uuid)
{
}

void HelloSession::Recved(const char* data, size_t size)
{
	m_sizeRecved += size;
	SPDLOG_INFO("Read Size[{}] All Read[{}]", size, m_sizeRecved);
	Send(data, size);
}

