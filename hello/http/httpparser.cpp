#include "httpparser.h"

#include "utils/UtilsSpdlog.h"
#include "utils/UrlPath.h"

HttpParser::HttpParser(bool is_resp)
{
}

int HttpParser::InputData(const char* data, size_t* size)
{
    m_method.clear();
    m_uri.clear();
    m_version.clear();
    m_body.clear();

	http_parser_t* parser = nullptr;

	if (m_ptrParser == nullptr) {
		m_ptrParser = std::make_shared<http_parser_t>();
		parser = m_ptrParser.get();
		http_parser_init(0, parser);
	}
	else {
		parser = m_ptrParser.get();
	}

	int nRet = http_parser_append_message(data, size, parser);
	if (nRet == 0) {
		// 没接收完，需要继续接收数据
		return 0;
	}

	if (nRet > 0) {
		m_method = http_parser_get_method(parser);
		m_uri = http_parser_get_uri(parser);
		m_version = http_parser_get_version(parser);

		CUrlPath path(m_uri);
		m_path = path.Path();

		for (auto& it : path.Querys()) {
			m_query.emplace(it.first, it.second);
		}

		const void* body;
		size_t body_len;
		int nRet = http_parser_get_body(&body, &body_len, parser);
		if (nRet == 0 && body_len) {
			m_body = std::string((const char*)body, body_len);
		}

		http_header_cursor_t cursor;
		http_header_cursor_init(&cursor, parser);

		const void* name;
		size_t name_len;
		const void* value;
		size_t value_len;

		while (http_header_cursor_next(&name, &name_len, &value, &value_len, &cursor) == 0) {
			std::string n((const char*)name, name_len);
			std::string v((const char*)value, value_len);
			m_header[n] = v;
		}

		http_header_cursor_deinit(&cursor);
	}
	else {
		SPDLOG_ERROR("Http Request Parser Failed, {}", nRet);
	}

	http_parser_deinit(parser);
	m_ptrParser = nullptr;

	return nRet;
}
