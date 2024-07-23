#pragma once

#include "../upstox.h"
#include <fmt/base.h>

namespace upstox_api {
inline upstox::upstox(string apikey, string redirect_uri)
    : key(std::move(apikey)), redirect_uri(std::move(redirect_uri)),
      client(root.c_str()) {}

inline void upstox::setApiKey(const string& arg) { key = arg; }

inline string upstox::getApiKey() const { return key; };

inline string upstox::loginURL() const {
    string encodedUrl = upstox::urlEncode(redirect_uri);
    return fmt::format(loginUrlFmt, fmt::arg("client_id", key),
        fmt::arg("redirect_uri", encodedUrl));
}

inline void upstox::setAccessToken(const string& arg) {
    code = arg;
    authorization = fmt::format("Bearer {0}", code);
}

inline string upstox::getAccessToken() const { return code; }

inline userSession upstox::generateSession(
    const string& requestCode, const string& apiSecret) {
    return callApi<userSession, nlohmann::json::object_t>("api.token",
        { { "client_id", key }, { "code", requestCode },
            { "grant_type", "authorization_code" },
            { "client_secret", apiSecret },
            { "redirect_uri", "https%3A%2F%2F127.0.0.1%3A5000%2F" } });
}

} // namespace upstox_api
