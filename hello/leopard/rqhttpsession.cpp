#include "rqhttpsession.h"

#include "utils/UtilsSpdlog.h"
#include "utils/UtilsJson.h"
#include "utils/UtilsTimes.h"
#include "utils/UtilsCommon.h"

#include "http/httpresponse.h"

RQHttpSession::RQHttpSession(int64_t uuid)
	: RQTcpSession(uuid)
{
	m_funcs["HEAD"] = BIND0(&RQHttpSession::OnHead);
	m_funcs["GET"] = BIND0(&RQHttpSession::OnGet);
	m_funcs["PUT"] = BIND0(&RQHttpSession::OnPut);
	m_funcs["POST"] = BIND0(&RQHttpSession::OnPost);
	m_funcs["DELETE"] = BIND0(&RQHttpSession::OnDelete);
}

RQHttpSession::~RQHttpSession()
{
}

void RQHttpSession::Recved(const char* data, size_t size)
{
	size_t left = size;
	while (left) {
		int nRet = m_req.InputData(data, &size);
		if (nRet == 0) {       // 需要更多数据
			return;            
		}
		else if (nRet > 0) {   // 完整的解析，回调一次请求
			OnHttpRequest();
		}
		else {                 // 错误请求
			OnHttpRequestError(nRet);
			return;
		}
		left -= size;
	}
}

void RQHttpSession::OnHttpRequestError(int nError)
{
}

void RQHttpSession::OnHttpRequest()
{
	SPDLOG_INFO("Req method[{}] url[{}]", m_req.m_method, m_req.m_uri);

	auto it = m_funcs.find(m_req.m_method);

	if (it != m_funcs.end()) {
		it->second();
		return;
	}

	nlohmann::json js;
	js["method"] = m_req.m_method;
	js["uri"] = m_req.m_uri;

	RQHttpResponse respon(m_req.m_version, 200, "OK");
	respon.AddHeaderPair("Date", GetFormattedTime());
	respon.SetJsonBody(js.dump(4));
	Send(respon.ToString());
}

void RQHttpSession::OnHead()
{
	RQHttpResponse respon(m_req.m_version, 200, "OK");
	respon.AddHeaderPair("Date", GetFormattedTime());
	respon.AddHeaderPair("Server", "www.rouchie.com");
	Send(respon.ToString());
}

void RQHttpSession::OnGet()
{
	RQHttpResponse respon(m_req.m_version, 200, "OK");
	respon.AddHeaderPair("Date", GetFormattedTime());
	respon.AddHeaderPair("Server", "www.rouchie.com");

	nlohmann::json js;
	js["Date"] = GetFormattedTime();

	respon.SetJsonBody(js.dump(4));

	Send(respon.ToString());
}

void RQHttpSession::OnPut()
{
	OnGet();
}

void RQHttpSession::OnPost()
{
	OnGet();
}

void RQHttpSession::OnDelete()
{
	OnGet();
}

void RQHttpSession::Response(int code, const std::string& message, nlohmann::json body)
{
	RQHttpResponse respon(m_req.m_version, code, message);
	respon.SetJsonBody(body.dump());
	Send(respon.ToString());
}

