#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "duckdb/common/http_util.hpp"

namespace duckdb {
class HTTPLogger;
class FileOpener;
struct FileOpenerInfo;

struct HTTPSParams final : HTTPParams {
	explicit HTTPSParams(HTTPUtil &http_util) : HTTPParams(http_util) {
	}

	static constexpr bool DEFAULT_ENABLE_SERVER_CERT_VERIFICATION = false;
	static constexpr uint64_t DEFAULT_HF_MAX_PER_PAGE = 0;

	bool enable_server_cert_verification = DEFAULT_ENABLE_SERVER_CERT_VERIFICATION;
	string ca_cert_file;
	string bearer_token;
};

class HTTPSUtil final : public HTTPUtil {
public:
	unique_ptr<HTTPParams> InitializeParameters(optional_ptr<FileOpener> opener,
	                                            optional_ptr<FileOpenerInfo> info) override;
	unique_ptr<HTTPClient> InitializeClient(HTTPParams &http_params, const string &proto_host_port) override;

	static unordered_map<string, string> ParseGetParameters(const string &text);

	string GetName() const override;
};

} // namespace duckdb
