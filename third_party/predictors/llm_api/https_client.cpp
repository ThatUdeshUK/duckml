#include "include/https_client.hpp"
#include "duckdb/common/file_opener.hpp"
#include "duckdb/main/secret/secret_manager.hpp"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.hpp"

namespace duckdb {

class HTTPSClient final : public HTTPClient {
public:
	HTTPSClient(const HTTPSParams &http_params, const string &proto_host_port) {
		client = make_uniq<duckdb_httplib_openssl::Client>(proto_host_port);
		client->set_follow_location(http_params.follow_location);
		client->set_keep_alive(http_params.keep_alive);
		if (!http_params.ca_cert_file.empty()) {
			client->set_ca_cert_path(http_params.ca_cert_file.c_str());
		}
		client->enable_server_certificate_verification(http_params.enable_server_cert_verification);
		client->set_write_timeout(http_params.timeout, http_params.timeout_usec);
		client->set_read_timeout(http_params.timeout, http_params.timeout_usec);
		client->set_connection_timeout(http_params.timeout, http_params.timeout_usec);
		client->set_decompress(false);
		if (!http_params.bearer_token.empty()) {
			client->set_bearer_token_auth(http_params.bearer_token.c_str());
		}

		if (!http_params.http_proxy.empty()) {
			client->set_proxy(http_params.http_proxy, http_params.http_proxy_port);

			if (!http_params.http_proxy_username.empty()) {
				client->set_proxy_basic_auth(http_params.http_proxy_username, http_params.http_proxy_password);
			}
		}
	}

	unique_ptr<HTTPResponse> Get(GetRequestInfo &info) override {
		const auto headers = TransformHeaders(info.headers, info.params);
		if (!info.response_handler && !info.content_handler) {
			return TransformResult(client->Get(info.path, headers));
		}

		return TransformResult(client->Get(
		    info.path.c_str(), headers,
		    [&](const duckdb_httplib_openssl::Response &response) {
			    const auto http_response = TransformResponse(response);
			    return info.response_handler(*http_response);
		    },
		    [&](const char *data, size_t const data_length) {
			    return info.content_handler(const_data_ptr_cast(data), data_length);
		    }));
	}
	unique_ptr<HTTPResponse> Put(PutRequestInfo &info) override {
		const auto headers = TransformHeaders(info.headers, info.params);
		return TransformResult(client->Put(info.path, headers, const_char_ptr_cast(info.buffer_in), info.buffer_in_len,
		                                   info.content_type));
	}

	unique_ptr<HTTPResponse> Head(HeadRequestInfo &info) override {
		const auto headers = TransformHeaders(info.headers, info.params);
		return TransformResult(client->Head(info.path, headers));
	}

	unique_ptr<HTTPResponse> Delete(DeleteRequestInfo &info) override {
		const auto headers = TransformHeaders(info.headers, info.params);
		return TransformResult(client->Delete(info.path, headers));
	}

	unique_ptr<HTTPResponse> Post(PostRequestInfo &info) override {
		// We use a custom Request method here, because there is no Post call with a contentreceiver in httplib
		duckdb_httplib_openssl::Request req;
		req.method = "POST";
		req.path = info.path;
		req.headers = TransformHeaders(info.headers, info.params);
		req.headers.emplace("Content-Type", "application/json");
		req.content_receiver = [&](const char *data, const size_t data_length, uint64_t /*offset*/,
		                           uint64_t /*total_length*/) {
			info.buffer_out += string(data, data_length);
			return true;
		};
		req.body.assign(const_char_ptr_cast(info.buffer_in), info.buffer_in_len);
		return TransformResult(client->send(req));
	}

private:
	static duckdb_httplib_openssl::Headers TransformHeaders(const HTTPHeaders &header_map, const HTTPParams &params) {
		duckdb_httplib_openssl::Headers headers;
		for (auto &entry : header_map) {
			headers.insert(entry);
		}
		for (auto &entry : params.extra_headers) {
			headers.insert(entry);
		}
		return headers;
	}

	static unique_ptr<HTTPResponse> TransformResponse(const duckdb_httplib_openssl::Response &response) {
		auto status_code = HTTPUtil::ToStatusCode(response.status);
		auto result = make_uniq<HTTPResponse>(status_code);
		result->body = response.body;
		result->reason = response.reason;
		for (const auto &[key, value] : response.headers) {
			result->headers.Insert(key, value);
		}
		return result;
	}

	static unique_ptr<HTTPResponse> TransformResult(duckdb_httplib_openssl::Result &&res) {
		if (res.error() == duckdb_httplib_openssl::Error::Success) {
			const auto &response = res.value();
			return TransformResponse(response);
		}

		auto result = make_uniq<HTTPResponse>(HTTPStatusCode::INVALID);
		result->request_error = to_string(res.error());
		return result;
	}

	unique_ptr<duckdb_httplib_openssl::Client> client;
};

unique_ptr<HTTPParams> HTTPSUtil::InitializeParameters(optional_ptr<FileOpener> opener,
                                                       optional_ptr<FileOpenerInfo> info) {
	auto result = make_uniq<HTTPSParams>(*this);
	result->Initialize(opener);

	// No point in continueing without an opener
	if (!opener) {
		return std::move(result);
	}

	Value value;

	// Setting lookups
	FileOpener::TryGetCurrentSetting(opener, "http_timeout", result->timeout, info);
	FileOpener::TryGetCurrentSetting(opener, "http_retries", result->retries, info);
	FileOpener::TryGetCurrentSetting(opener, "http_retry_wait_ms", result->retry_wait_ms, info);
	FileOpener::TryGetCurrentSetting(opener, "http_retry_backoff", result->retry_backoff, info);
	FileOpener::TryGetCurrentSetting(opener, "http_keep_alive", result->keep_alive, info);
	FileOpener::TryGetCurrentSetting(opener, "enable_server_cert_verification", result->enable_server_cert_verification,
	                                 info);
	FileOpener::TryGetCurrentSetting(opener, "ca_cert_file", result->ca_cert_file, info);

	// HTTP Secret lookups
	KeyValueSecretReader settings_reader(*opener, info, "http");

	if (string proxy_setting;
	    settings_reader.TryGetSecretKey<string>("http_proxy", proxy_setting) && !proxy_setting.empty()) {
		idx_t port;
		string host;
		ParseHTTPProxyHost(proxy_setting, host, port);
		result->http_proxy = host;
		result->http_proxy_port = port;
	}
	settings_reader.TryGetSecretKey<string>("http_proxy_username", result->http_proxy_username);
	settings_reader.TryGetSecretKey<string>("http_proxy_password", result->http_proxy_password);
	settings_reader.TryGetSecretKey<string>("bearer_token", result->bearer_token);

	if (Value extra_headers; settings_reader.TryGetSecretKey("extra_http_headers", extra_headers)) {
		auto children = MapValue::GetChildren(extra_headers);
		for (const auto &child : children) {
			auto kv = StructValue::GetChildren(child);
			D_ASSERT(kv.size() == 2);
			result->extra_headers[kv[0].GetValue<string>()] = kv[1].GetValue<string>();
		}
	}

	return std::move(result);
}

unique_ptr<HTTPClient> HTTPSUtil::InitializeClient(HTTPParams &http_params, const string &proto_host_port) {
	auto client = make_uniq<HTTPSClient>(http_params.Cast<HTTPSParams>(), proto_host_port);
	return std::move(client);
}

unordered_map<string, string> HTTPSUtil::ParseGetParameters(const string &text) {
	duckdb_httplib_openssl::Params query_params;
	duckdb_httplib_openssl::detail::parse_query_text(text, query_params);

	unordered_map<string, string> result;
	for (auto &entry : query_params) {
		result.emplace(std::move(entry.first), std::move(entry.second));
	}
	return result;
}

string HTTPSUtil::GetName() const {
	return "HTTPS";
}

} // namespace duckdb
