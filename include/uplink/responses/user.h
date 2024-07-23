#pragma once

#include <string>

#include "../parser.h"
#include "nlohmann/json.hpp"

namespace upstox_api {
using std::string;
struct userProfile {
    userProfile() = default;
    explicit userProfile(const nlohmann::json& val) { parse(val); };
    void parse(const nlohmann::json& val) {
        userId = parser::get<std::string>(val, "user_id");
        userName = parser::get<std::string>(val, "user_name");
        email = parser::get<std::string>(val, "email");
        userType = parser::get<std::string>(val, "user_type");
        broker = parser::get<std::string>(val, "broker");
        products = parser::get<std::vector<std::string>>(val, "products");
        orderTypes = parser::get<std::vector<std::string>>(val, "order_types");
        exchanges = parser::get<std::vector<std::string>>(val, "exchanges");
    };
    std::string userId;
    std::string userName;
    std::string userType;
    std::string email;
    std::string broker;
    std::vector<std::string> products;
    std::vector<std::string> orderTypes;
    std::vector<std::string> exchanges;
};

struct userTokens {
    userTokens() = default;
    explicit userTokens(const nlohmann::json& val) { parse(val); };

    void parse(const nlohmann::json& val) {
        userId = parser::get<std::string>(val, "user_id");
        accessToken = parser::get<std::string>(val, "access_token");
    }
    std::string userId;
    std::string accessToken;
};

struct userSession {
    userSession() = default;
    explicit userSession(const nlohmann::json& val) { parse(val); };

    void parse(const nlohmann::json& val) {
        profile.parse(val);
        tokens.parse(val);
    }
    userProfile profile;
    userTokens tokens;
};

struct margins {
    margins() = default;
    explicit margins(const nlohmann::json& val) { parse(val); };
    void parse(const nlohmann::json& val) {
        usedMargin = parser::get<double>(val, "user_margin");
        payinAmount = parser::get<double>(val, "payin_amount");
        spanMargin = parser::get<double>(val, "span_margin");
        adhocMargin = parser::get<double>(val, "adhoc_margin");
        notionalCash = parser::get<double>(val, "notional_cash");
        availableMargin = parser::get<double>(val, "available_margin");
        exposureMargin = parser::get<double>(val, "exposure_margin");
    }
    double usedMargin;
    double payinAmount;
    double spanMargin;
    double adhocMargin;
    double notionalCash;
    double availableMargin;
    double exposureMargin;
};

struct allMargins {
  allMargins() = default;
  explicit allMargins(const nlohmann::json& val) { parse(val); };
  void parse(const nlohmann::json& val){
    equity = parser::get<parser::JsonObject, margins>(val, "equity");
    commodity = parser::get<parser::JsonObject, margins>(val, "commodity");
  }
    margins equity;
    margins commodity;
};
} // namespace upstox_api
