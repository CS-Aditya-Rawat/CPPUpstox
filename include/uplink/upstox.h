#pragma once

#include "http.h"
#include "httplib.h"

#include "responses/responses.h"

#include <curl/curl.h>
#include <fmt/format.h>
#include <string>
#include <unordered_map>

namespace upstox_api {
using std::string;

class upstox {
  public:
    explicit upstox(string apiKey, string redirect_uri);
    void setApiKey(const string& arg);
    string getApiKey() const;
    string loginURL() const;
    void setAccessToken(const string& arg);
    string getAccessToken() const;
    userSession generateSession(const string& code, const string& apiSecret);

    /*
     *@brief Get user's profile.
     *@return userProfile user profile
     */
    userProfile profile();
    allMargins getMargins();
    margins getMargins(const string& segment);

  // orders
   // string placeOrder(const placeOrderParams& params);

    string urlEncode(const string& url) const {
        CURL* curl = curl_easy_init();
        if (curl) {
            char* encoded = curl_easy_escape(curl, url.c_str(), url.length());
            if (encoded) {
                string encodedUrl(encoded);
                curl_free(encoded);
                curl_easy_cleanup(curl);
                return encodedUrl;
            }
            curl_easy_cleanup(curl);
        }
        return "";
    }

  private:
    string getAuth() const;

    template <class Res, class Data, bool UseCustomParser = false>
    inline Res callApi(const string& service, const http::Params& body = {},
        const std::vector<string>& fmtArgs = {},
        parser::CustomParser<Res, Data, UseCustomParser> customParser = {});

    const string root = "https://api.upstox.com";

    const string loginUrlFmt = "https://api.upstox.com/v2/login/authorization/"
                               "dialog?response_type=code&client_id={client_id}"
                               "&redirect_uri={redirect_uri}";

    const std::unordered_map<string, http::endpoint> endpoints { // api
        { "api.token", { http::METHOD::POST, "/v2/login/authorization/token" } },
        // user
        { "user.profile", { http::METHOD::GET, "/v2/user/profile" } },
        { "user.margin", { http::METHOD::GET, "/v2/user/get-funds-and-margin" } },
        { "user.margin.segment",
            { http::METHOD::GET, "/v2/user/get-funds-and-margin?segment={}" } }
    };
    string key;
    string redirect_uri;
    string code;
    string authorization;
    httplib::Client client;

  protected:
    http::response sendReq(const http::endpoint& endpoint,
        const http::Params& body, const std::vector<string>& fmtArgs);
};
} // namespace upstox_api
