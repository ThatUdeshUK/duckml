// The MIT License (MIT)
// 
// Copyright (c) 2023 Olrea, Florian Dang
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef OPENAI_HPP_
#define OPENAI_HPP_

#if OPENAI_VERBOSE_OUTPUT
#pragma message ("OPENAI_VERBOSE_OUTPUT is ON")
#endif

#include "https_client.hpp"
#include "duckdb/common/exception/http_exception.hpp"
#include "duckdb/main/database_file_opener.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <mutex>
#include <cstdlib>
#include <map>

#include "../../common/nlohmann/json.hpp"  // nlohmann/json

namespace openai {

namespace _detail {

// Json alias
using Json = nlohmann::json;

// OpenAI
class OpenAI {
private:
    duckdb::HTTPSUtil       http_util_ = duckdb::HTTPSUtil();
    duckdb::HTTPHeaders     headers_;
    std::unique_ptr<duckdb::HTTPParams> params_;
    std::string             token_;
    std::string base_url_;
    std::string             organization_;
    bool                    throw_exception_;

public:
    OpenAI(duckdb::DatabaseInstance &db, const std::string& token = "", const std::string& organization = "", bool throw_exception = true, const std::string& api_base_url = "", const std::string& beta = "") 
        : token_{token}, base_url_{api_base_url}, organization_{organization}, throw_exception_{throw_exception} {
            if (token.empty()) {
                if(const char* env_p = std::getenv("OPENAI_API_KEY")) {
                    token_ = std::string{env_p};
                }
            }

            if (api_base_url.empty()) {
                if(const char* env_p = std::getenv("OPENAI_API_BASE")) {
                    base_url_ = std::string{env_p} + "/";
                }
                else {
                    base_url_ = api_base_url;
                }
            }

            duckdb::DatabaseFileOpener opener{db};
            duckdb::FileOpenerInfo info;
            info.file_path = base_url_;
            params_ = http_util_.InitializeParameters(&opener, &info);
        }
    
    OpenAI(const OpenAI&)               = delete;
    OpenAI& operator=(const OpenAI&)    = delete;
    OpenAI(OpenAI&&)                    = delete;
    OpenAI& operator=(OpenAI&&)         = delete;

    void setToken(const std::string& token) { 
        token_ = token; 
        headers_.Insert("Authorization", duckdb::StringUtil::Format("Bearer %s", token_));
    };

    void setThrowException(bool throw_exception) { throw_exception_ = throw_exception; }

    Json post(const std::string& suffix, const std::string& data, const std::string& contentType) {
        // Unclear what's peculiar about extension install flow, but those two parameters are needed
        // to avoid lengthy retry on 304
        params_->follow_location = false;
        params_->keep_alive = false;

        auto url = base_url_ + suffix;
        #if OPENAI_VERBOSE_OUTPUT
            std::cout << "<< request: "<< url << "  " << data << '\n';
        #endif

        duckdb::PostRequestInfo post_request(url, headers_, *params_, duckdb::const_data_ptr_cast(data.c_str()), data.size());
        post_request.try_request = true;

        auto response = http_util_.Request(post_request);
        if (!response->Success()) {
            if (response->HasRequestError()) {
                // request error - this means something went wrong performing the request
                // throw duckdb::IOException("Failed call LLM at URL \"%s\"\n (ERROR %s)", url,
                //                            response->GetRequestError());
                Json out{};
                out["error"] = response->GetRequestError();
                out["code"] = -1;
                return out;
            }
            // // if this was not a request error this means the server responded - report the response status and response
            // throw duckdb::HTTPException(*response, "Failed to call LLM at URL \"%s\" (HTTP %n) - %s: Request: %s\n",
            //                             url, int(response->status), response->reason, data);
            Json out{};
            out["error"] = response->reason;
            out["code"] = int(response->status);
            return out;
        }

        Json json{};
        if (isJson(post_request.buffer_out)){
            json = Json::parse(post_request.buffer_out); 
        } else {
          #if OPENAI_VERBOSE_OUTPUT
            std::cerr << "Response is not a valid JSON";
            std::cout << "<< " << response.text << "\n";
          #endif
        }
       
        return json;
    }

    Json post(const std::string& suffix, const Json& json, const std::string& contentType="application/json") {
        return post(suffix, json.dump(), contentType);
    }

    void debug() const { std::cout << token_ << '\n'; }

    void setBaseUrl(const std::string &url) {
        base_url_ = url;
    }

    std::string getBaseUrl() const {
        return base_url_;
    }

private:
    void checkResponse(const Json& json) {
        if (json.count("error")) {
            auto reason = json["error"].dump();
            trigger_error(reason);

            #if OPENAI_VERBOSE_OUTPUT
                std::cerr << ">> response error :\n" << json.dump(2) << "\n";
            #endif
        } 
    }

    // as of now the only way
    bool isJson(const std::string &data){
        bool rc = true;
        try {
            auto json = Json::parse(data); // throws if no json 
        }
        catch (std::exception &){
            rc = false;
        }
        return(rc);
    }

    void trigger_error(const std::string& msg) {
        if (throw_exception_) {
            throw std::runtime_error(msg);
        }
        else {
            std::cerr << "[OpenAI] error. Reason: " << msg << '\n';
        }
    }
};

inline std::string bool_to_string(const bool b) {
    std::ostringstream ss;
    ss << std::boolalpha << b;
    return ss.str();
}

inline OpenAI& start(duckdb::DatabaseInstance &db, const std::string& api_base_url = "", const std::string& token = "", const std::string& organization = "", bool throw_exception = true)  {
    static OpenAI instance{db, token, organization, throw_exception, api_base_url};
    instance.setBaseUrl(api_base_url);
    instance.setToken(token);
    return instance;
}

} // namespace _detail

// Public interface
using _detail::OpenAI;

// instance
using _detail::start;

using _detail::Json;

} // namespace openai

#endif // OPENAI_HPP_
