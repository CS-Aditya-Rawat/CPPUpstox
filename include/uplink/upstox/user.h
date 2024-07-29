#include "../parser.h"
#include "../upstox.h"

namespace upstox_api {
inline userProfile upstox::profile() {
    return callApi<userProfile, parser::JsonObject>("user.profile");
};

inline allMargins upstox::getMargins() {
    return callApi<allMargins, parser::JsonObject>("user.margin");
}

inline margins upstox::getMargins(const string& segment) {
    return callApi<margins, parser::JsonObject>(
        "user.margin.segment", {}, { segment });
}
} // namespace upstox_api
