#pragma once

#include "uplink/parser.h"
#include <exception>
#include <string>
#include <utility>

namespace upstox_api {
using std::string;

class uplinkException : public std::exception {
  public:
    uplinkException(int Code_, string Message_)
        : Code(Code_), Message(std::move(Message_)) {};
    [[nodiscard]] const char* what() const noexcept override = 0;
    [[nodiscard]] int code() const noexcept { return Code; }
    [[nodiscard]] const char* message() const noexcept {
        return Message.c_str();
    }

  private:
    int Code = 0;
    string Message;
};

// @brief Represents user account related error.
class userException : public uplinkException {
  public:
    userException(int code, string message)
        : uplinkException(code, std::move(message)) {};

    [[nodiscard]] const char* what() const noexcept override {
        return "UserException was thrown by Upstox";
    }
};

class tokenException : public uplinkException {
  public:
    tokenException(int code, string message)
        : uplinkException(code, std::move(message)) {};

    [[nodiscard]] const char* what() const noexcept override {
        return "TokenException was throw by Upstox";
    }
};

namespace internal {
inline void throwException(
    const string& exceptionString, int code, const string& msg) {
    // exception strings sent by API
    static const string tokenExceptionString = "TokenException";
    static const string userExceptionString = "UserException";

    // when rest api doesn't send an exception but http status code isn't 200
    // either
    static const string unknownExceptionString = "NoException";
    if (exceptionString == tokenExceptionString) {
        throw tokenException(code, msg);
    }
    throw libException("unknown exception was thrown by REST API");
};
} // namespace internal
} // namespace upstox_api
