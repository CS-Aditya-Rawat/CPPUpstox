#pragma once

#include "fmt/args.h"
#include "fmt/format.h"
#include "httplib.h"
#include "parser.h"
#include <cstdint>
#include <fmt/base.h>

#define FMT fmt::format

namespace http {
namespace code {
constexpr uint16_t OK = 200;
}

enum class METHOD : uint8_t {
    GET,
    POST,
    DEL,
    HEAD
};

enum class CONTENT_TYPE : uint8_t {
    JSON,
    NON_JSON
};

using Params = httplib::Params;

struct endpoint {
    bool operator==(const endpoint& lhs) const {
        return lhs.method == this->method && lhs.Path.Path == this->Path.Path &&
               lhs.contentType == this->contentType;
    }
    METHOD method = METHOD::GET;

    struct path {
        std::string operator()(
            const std::vector<std::string>& fmtArgs = {}) const {
            if (!fmtArgs.empty()) {
                fmt::dynamic_format_arg_store<fmt::format_context> store;
                for (const auto& arg : fmtArgs) { store.push_back(arg); };
                return fmt::vformat(Path, store);
            }
            return Path;
        };
        std::string Path;
    } Path;
    CONTENT_TYPE contentType = CONTENT_TYPE::NON_JSON;
    CONTENT_TYPE responseType = CONTENT_TYPE::JSON;
};

class response {
  public:
    response(uint16_t Code, const std::string& body, bool json = true)
        : code(Code) {
        parse(Code, body, json);
    };
    ;
    explicit operator bool() const { return !error; };
    uint16_t code = 0;   /// http code
    bool error = false;  /// true if kite api reported an error (a status field)
    nlohmann::json data; /// parsed body
    std::string errorType =
        "NoException";   /// corresponds to kite api's \a error_type field (if a
                         /// error is \a true)
    std::string message; /// corresponds to kite api's \a message field (if a
                         /// error is a true)
    std::string rawBody; /// raw body, set in case of non-json response
  private:
    void parse(uint16_t code, const std::string& body, bool json) {
        if (json) {
            parser::parse(data, body);
            if (code != static_cast<uint16_t>(code::OK)) {
                std::string status;
                status =
                    parser::get<std::string, nlohmann::json>(data, "status");
                errorType = parser::get<std::string, nlohmann::json>(
                    data, "error_type");
                message =
                    parser::get<std::string, nlohmann::json>(data, "message");
                if (status != "success") { error = true; };
            }
        } else {
            if (code != static_cast<uint16_t>(code::OK)) { error = true; };
            rawBody = body;
        }
    }
};

std::string params_to_string(const httplib::Params& params) {
    std::ostringstream oss;
    bool first = true;

    for (const auto& param : params) {
        if (!first) { oss << "&"; }
        first = false;
        oss << param.first << "=" << param.second;
    }
    return oss.str();
}

struct request {
    response send(httplib::Client& client) const {
        const httplib::Headers headers = { { "Authorization", authToken } };

        uint16_t code = 0;
        std::string data;

        switch (method) {
            case http::METHOD::GET:
                if (auto res = client.Get(path, headers)) {
                    code = res->status;
                    data = res->body;
                } else {
                    throw libException(FMT("request failed (){0})",
                        httplib::to_string(res.error())));
                }
                break;
            case http::METHOD::POST:
                if (contentType != CONTENT_TYPE::JSON) {
                    if (auto res = client.Post(path,
                            { { "Content-Type",
                                  "application/x-www-form-urlencoded" },
                                { "Accept", "application/json" } },
                            params_to_string(body),
                            "application/x-www-form-urlencoded")) {
                        code = res->status;
                        data = res->body;
                    } else {
                        throw libException(FMT("request failed ({0})",
                            httplib::to_string(res.error())));
                    }
                } else {
                    if (auto res = client.Post(path, headers, serializedBody,
                            "application/json")) {
                        code = res->status;
                        data = res->body;
                    } else {
                        throw libException(FMT("request failed ({0})",
                            httplib::to_string(res.error())));
                    }
                }
                break;
            case http::METHOD::DEL:
                if (auto res = client.Delete(path, headers)) {
                    code = res->status;
                    data = res->body;
                } else {
                    throw libException(FMT("request failed ({0})",
                        httplib::to_string(res.error())));
                }
                break;
            default: throw libException("unsupported http method");
        };
        return { code, data, responseType == CONTENT_TYPE::JSON };
    };
    http::METHOD method;
    std::string path;
    std::string authToken;
    Params body;
    CONTENT_TYPE contentType = CONTENT_TYPE::NON_JSON;
    CONTENT_TYPE responseType = CONTENT_TYPE::JSON;
    std::string serializedBody;
};
} // namespace http

template <class Param>
void addParam(
    http::Params& bodyParams, Param& param, const std::string& fieldName) {
    static_assert(
        isOptional<std::decay_t<Param>>::value, "Param must be std::optional");
    if (param.has_value()) {
        std::string fieldValue;
        if constexpr (!std::is_same_v<typename Param::value_type,
                          std::string>) {
            fieldValue = std::to_string(param.value());
        } else {
            fieldValue = param.value();
        }
        if (param.has_value()) { bodyParams.emplace(fieldName, fieldValue); }
    }
};
