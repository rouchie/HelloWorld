#include "httpparser.h"

#include "utils/UtilsSpdlog.h"
#include "utils/UrlPath.h"

static int on_message_begin(http_parser *parser)
{
    auto *http_parser = static_cast<HttpParser *>(parser->data);
    http_parser->m_response = std::make_shared<RQHttpResponse>();
    http_parser->m_is_complete = false;
    return 0;
}

static int on_url(http_parser *parser, const char *at, size_t length)
{
    SPDLOG_INFO("at[{}] length[{}]", at, length);
    return 0;
}

static int on_status(http_parser *parser, const char *at, size_t length)
{
    auto *http_parser = static_cast<HttpParser *>(parser->data);

    auto f = [](const unsigned short type) {
        switch (type) {
            case HTTP_VERSION: return "HTTP";
            case RTSP_VERSION: return "RTSP";
            case SIP_VERSION: return "SIP";
            default: return "";
        }
    };

    http_parser->m_response->SetVersion(fmt::format("{}/{}.{}", f(parser->http_type), parser->http_major, parser->http_minor));
    http_parser->m_response->SetStatusCode(parser->status_code);
    http_parser->m_response->SetStatusPhrase(std::string(at, length));
    return 0;
}

static int on_header_field(http_parser *parser, const char *at, size_t length)
{
    auto *http_parser = static_cast<HttpParser *>(parser->data);
    http_parser->m_header_key = std::string(at, length);
    return 0;
}

static int on_header_value(http_parser *parser, const char *at, size_t length)
{
    auto *http_parser = static_cast<HttpParser *>(parser->data);
    http_parser->m_response->AddHeaderPair(http_parser->m_header_key, std::string(at, length));
    return 0;
}

static int on_headers_complete(http_parser *parser)
{
    return 0;
}

static int on_body(http_parser *parser, const char *at, size_t length)
{
    auto *http_parser = static_cast<HttpParser *>(parser->data);
    http_parser->m_response->SetBody(std::string(at, length));
    return 0;
}

static int on_message_complete(http_parser *parser)
{
    auto *http_parser = static_cast<HttpParser *>(parser->data);
    http_parser->m_is_complete = true;
    http_parser->OnHttpResponse(http_parser->m_response);
    return 0;
}

static int on_chunk_header(http_parser *parser)
{
    return 0;
}

static int on_chunk_complete(http_parser *parser)
{
    return 0;
}

static constexpr http_parser_settings settings = {
    on_message_begin,
    on_url,
    on_status,
    on_header_field,
    on_header_value,
    on_headers_complete,
    on_body,
    on_message_complete,
    on_chunk_header,
    on_chunk_complete
};

HttpParser::HttpParser(bool is_resp)
    : m_is_resp(is_resp)
{
    m_ptr_parser = std::make_shared<http_parser>();
    http_parser_init(m_ptr_parser.get(), is_resp ? HTTP_RESPONSE : HTTP_REQUEST);
    m_ptr_parser->data = this;
}

int HttpParser::InputData(const char *data, size_t *size) const
{
    const size_t n = http_parser_execute(m_ptr_parser.get(), &settings, data, *size);

    if (m_ptr_parser->http_errno) {
        SPDLOG_INFO("{}", http_errno_name(static_cast<http_errno>(m_ptr_parser->http_errno)));
        return -1;
    }

    *size = n;

    return m_is_complete;
}

void HttpParser::OnHttpResponse(RQHttpResponse::Ptr response)
{
    if (response) {
        SPDLOG_INFO("response: {}", response->ToString());
    }
}

#if 0
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
		http_parser_init(m_is_resp, parser);
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
		if (m_is_resp) {
			m_version = http_parser_get_version(parser);
			m_code = http_parser_get_code(parser);
			m_phrase = http_parser_get_phrase(parser);
		} else {
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
			const int r = http_parser_get_body(&body, &body_len, parser);
			if (r == 0 && body_len) {
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
	}
	else {
		SPDLOG_ERROR("Http Request Parser Failed, {}", nRet);
	}

	http_parser_deinit(parser);
	m_ptrParser = nullptr;

	return nRet;
}
#endif
