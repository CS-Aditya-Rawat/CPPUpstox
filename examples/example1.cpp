#include "uplink.h"

#include <cstdlib>
#include <iostream>

namespace ui = upstox_api;

int main() {
    ui::upstox Upstox(std::getenv("UPSTOX_API_KEY"), "https://127.0.0.1:5000/");
    std::cout << "API KEY: " << Upstox.getApiKey() << "\n";

    std::cout << Upstox.loginURL() << "\n";

    std::string reqCode;
    std::cout << "enter obtained request token: ";
    std::cin >> reqCode;

    std::string accessToken =
        Upstox.generateSession(reqCode, std::getenv("UPSTOX_API_SECRET"))
            .tokens.accessToken;

    Upstox.setAccessToken(accessToken);
    std::cout << "access token is " << Upstox.getAccessToken() << '\n';

    //! [get user profile]
    ui::userProfile profile = Upstox.profile();
    std::cout << profile.userName << "\n";
    std::cout << profile.email << "\n";

    //! [get margins]
    ui::allMargins margins = Upstox.getMargins();
    std::cout << "available cash for equity segment is "
              << margins.equity.availableMargin;

    ui::margins equityMargins = Upstox.getMargins("SEC");
    std::cout << "adhoc margin for equity segment is"
              << equityMargins.adhocMargin << "\n";
}
