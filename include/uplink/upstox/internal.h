#pragma once

#include <vector>

#include "../upstox.h"
#include "uplink/exceptions.h"

namespace upstox_api {
inline string upstox::getAuth() const { return authorization; }

inline http::response upstox::sendReq(const http::endpoint& endpoint,
    const http::Params& body, const std::vector<string>& fmtArgs) {
    if (endpoint.contentType == http::CONTENT_TYPE::JSON) {
        return http::request { endpoint.method, endpoint.Path(fmtArgs),
            getAuth(), body, endpoint.contentType, endpoint.responseType,
            body.begin()->second }
            .send(client);
    }
    return http::request { endpoint.method, endpoint.Path(fmtArgs), getAuth(),
        body, endpoint.contentType, endpoint.responseType }
        .send(client);
}

template <class Res, class Data, bool UseCustomParser>
inline Res upstox::callApi(const string& service, const http::Params& body,
    const std::vector<string>& fmtArgs,
    parser::CustomParser<Res, Data, UseCustomParser> customParser) {
    http::response res = sendReq(endpoints.at(service), body, fmtArgs);
    if (!res) {
        upstox_api::internal::throwException(
            res.errorType, res.code, res.message);
    }
    return parser::parse<Res, Data, UseCustomParser>(res.data, customParser);
}
} // namespace upstox_api
