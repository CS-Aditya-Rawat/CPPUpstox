#pragma once

#include <exception>
#include <fmt/base.h>
#include <nlohmann/detail/macro_scope.hpp>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "fmt/args.h"
#include "fmt/format.h"
#include "nlohmann/json.hpp"

class libException : public std::runtime_error {
  public:
    explicit libException(const std::string& message)
        : std::runtime_error(message) {}
};

//! init_list doesn't have compare operator & gmock needs it
template <typename>
struct isOptional : std::false_type {};
template <typename T>
struct isOptional<std::optional<T>> : std::true_type {};

template <typename>
struct isVector : std::false_type {};
template <typename T>
struct isVector<std::vector<T>> : std::true_type {};

namespace parser {

using json = nlohmann::json;
using string = std::string;

using JsonObject = nlohmann::json::object_t;
using JsonArray = nlohmann::json::array_t;
template <class Res>
using CustomObjectParser = std::function<Res(JsonObject&)>;
template <class Res>
using CustomArrayParser = std::function<Res(JsonArray&)>;
template <class Res, class Data, bool UseCustomParser>
using CustomParser = std::conditional_t<std::is_same_v<Data, JsonObject>,
    const CustomObjectParser<Res>&, const CustomArrayParser<Res>&>;
// JsonEncoder definition
template <typename T>
using JsonEncoder = std::function<void(const T&, nlohmann::json&)>;

inline JsonObject extractObject(const nlohmann::json& doc) {

    try {
        if (!doc.contains("data")) {
            // data is not exist in getting authorize token
            return doc.get<JsonObject>();
            throw libException("Key 'data' not found");
        }
        return doc.at("data").get<nlohmann::json>();
    } catch (std::exception& ex) { throw libException("invalid body"); }
}

inline JsonArray extractArray(const nlohmann::json& doc) {
    try {
        return doc.at("data").get<nlohmann::json::array_t>();
    } catch (const std::exception& ex) { throw libException("invalid body"); }
}

inline bool extractBool(const nlohmann::json& doc) {
    try {
        return doc.at("data").get<bool>();
    } catch (const std::exception& ex) { throw libException("invalid body"); }
}

inline std::string extractString(const nlohmann::json& doc) {
    try {
        return doc.at("data").get<std::string>();
    } catch (const std::exception& ex) { throw libException("invalid body"); }
}

inline bool parse(nlohmann::json& document, const std::string& str) {
    try {
        document = nlohmann::json::parse(str);
    } catch (const nlohmann::json::parse_error& e) {
        throw libException("failed to parse json string: " + str);
    }
    return true;
}

template <class Output, class Document = nlohmann::json>
inline Output get(const Document& val, const char* name) {

    static const auto exceptionString = [&name](const std::string& type) {
        return fmt::format("type of {0} not is not {1}", name, type);
    };
    if (val.contains(name)) {
        const auto& it = val.at(name);
        if constexpr (!isVector<Output>::value) {
            if constexpr (std::is_same_v<std::decay_t<Output>, std::string>) {
                if (it.is_string()) { return it.template get<std::string>(); }
                if (it.is_null()) { return ""; }
                throw libException(exceptionString("string"));
            } else if constexpr (std::is_same_v<std::decay_t<Output>, double>) {
                if (it.is_number_float()) { return it.template get<double>(); }
                if (it.is_number_integer()) {
                    return static_cast<double>(it.template get<int>());
                }
                throw libException(exceptionString("double"));
            }
        } else {
            if (it.is_array()) {
                Output out;
                for (const auto& v : it) {
                    if constexpr (std::is_same_v<
                                      std::decay_t<typename Output::value_type>,
                                      std::string>) {
                        if (v.is_string()) {
                            out.emplace_back(v.template get<std::string>());
                        } else {
                            throw libException(exceptionString("string"));
                        }
                    }
                }
                return out;
            }
            throw libException(exceptionString("array"));
        }
    } else {
        return {};
    }
}

template <class Val, class Output>
Output get(const nlohmann::json& val, const char* name) {
    static const auto exceptionString = [&name](const std::string& type) {
        return fmt::format("type of {0} not is not {1}", name, type);
    };

    if (val.contains(name)) {
        const auto& it = val.at(name);
        if constexpr (std::is_same_v<Val, nlohmann::json::object_t>) {
            static_assert(
                std::is_constructible_v<Output, nlohmann::json::object_t>);
            if (it.is_object()) {
                return Output(it.get<nlohmann::json::object_t>());
            }
            throw libException(exceptionString("object"));
        } else if constexpr (std::is_same_v<Val, nlohmann::json::array_t>) {
            static_assert(
                std::is_constructible_v<Output, nlohmann::json::array_t>);
            if (it.is_array()) {
                return Output(it.get<nlohmann::json::array_t>());
            }
            throw libException(exceptionString("array"));
        };
        return {};
    }
    return {};
}

template <class Val>
bool get(const nlohmann::json& val, nlohmann::json& out, const char* name) {
    static const auto exceptionString = [&name](const std::string& type) {
        return fmt::format("type of {0} not is not {1}", name, type);
    };
    if (val.contains(name)) {
        const auto& it = val.at(name);
        if constexpr (std::is_same_v<Val, nlohmann::json::object_t>) {
            if (it.is_object()) {
                out = it;
                return true;
            }
            throw libException(exceptionString("object"));
        } else if constexpr (std::is_same_v<Val, nlohmann::json::array_t>) {
            if (it.is_array()) {
                out = it;
                return true;
            }
            throw libException(exceptionString("array"));
        }
    }
    return false;
};

template <class Res, class Data, bool UseCustomParser>
Res parse(nlohmann::json& doc,
    CustomParser<Res, Data, UseCustomParser> customParser) {
    if constexpr (std::is_same_v<Data, JsonObject>) {
        auto object = extractObject(doc);
        if constexpr (UseCustomParser) {
            return customParser(object);
        } else {
            static_assert(std::is_constructible_v<Res, JsonObject>,
                "Res should be constructable using JsonObject");
            return Res(object);
        }
    } else if constexpr (std::is_same_v<Data, JsonArray>) {
        auto array = extractArray(doc);
        if constexpr (UseCustomParser) {
            return customParser(array);
        } else {
            static_assert(std::is_constructible_v<Res, JsonArray>,
                "Res should be constructable using JsonArray");
            return Res(array);
        }
    } else if constexpr (std::is_same_v<Data, bool>) {
        static_assert(std::is_same_v<Res, bool>,
            "Res needs to be bool if the data is bool");
        return extractBool(doc);
    }
}

template <class T>
class Json {
  public:
    Json() {
        if constexpr (std::is_same_v<T, json::object_t>) {
            dom = json::object();
        } else {
            dom = json::array();
        }
    }

    template <class Value>
    void field(
        const string& name, const Value& value, json* docOverride = nullptr) {

        if (docOverride == nullptr) {
            dom[name] = value;
        } else {
            (*docOverride)[name] = value;
        }
    }

    template <class Value>
    void field(const string& name, const std::vector<Value>& values,
        const JsonEncoder<Value>& encode = {}) {
        json arrayBuffer = json::array();
        for (const auto& i : values) {
            if constexpr (std::is_fundamental_v<std::decay_t<Value>>) {
                arrayBuffer.push_back(i);
            } else {
                json objectBuffer = json::object();
                if (encode) {
                    encode(i, objectBuffer);
                } else {
                    fmt::print(
                        "Encode function not provided for non-fundamental");
                }
                arrayBuffer.push_back(objectBuffer);
            }
        }
        if constexpr (std::is_same_v<T, JsonObject>) {
            dom[name] = arrayBuffer;
        } else {
            dom = arrayBuffer;
        }
    }

    template <class Value>
    void array(const std::vector<Value>& values,
        const JsonEncoder<Value>& encode = {}) {
        field("", values, encode);
    }

    string serialize() const { return dom.dump(); }

  private:
    json dom;
};
} // namespace parser
