//
// Created by Yancey on 2023/11/7.
//

#ifndef CHELPER_JSONUTIL_H
#define CHELPER_JSONUTIL_H

#include "pch.h"

namespace CHelper {

    class ErrorReason;

    namespace JsonUtil {

        class ConvertResult {
        public:
            std::string result;
            std::shared_ptr<ErrorReason> errorReason;
            std::vector<size_t> indexConvertList;
            bool isComplete = false;

            size_t convert(size_t index);

        };

        std::string string2jsonString(const std::string &input);

        ConvertResult jsonString2String(const std::string &input);

        nlohmann::json getJsonFromPath(const std::filesystem::path &path);

        template<class T>
        inline T fromJson(const nlohmann::json &json, const std::string &name) {
            return json.at(name).get<T>();
        }

        template<class T>
        std::optional<T> fromJsonOptional(const nlohmann::json &json, const std::string &name) {
            return json.contains(name) ? std::optional<T>(json.at(name).get<T>()) : std::nullopt;
        }

        template<class T>
        inline void toJson(nlohmann::json &json, const std::string &name, const T &data) {
            json[name] = data;
        }

        template<class T>
        void toJsonOptional(nlohmann::json json, const std::string &name, std::optional<T> data) {
            if (data.has_value()) {
                json[name] = data.value();
            }
        }

    } // JsonUtil

} // CHelper

// 让这个类支持json序列化（还需要在这个类中创建构造方法用于把json文本转为对象，创建一个to_json方法用于把对象转json文本）
#define CREATE_ADL_SERIALIZER(type)                                               \
template<>                                                                        \
struct [[maybe_unused]] nlohmann::adl_serializer<type> {                          \
                                                                                  \
    static type from_json(const nlohmann::json &j) {                              \
        return type(j);                                                           \
    }                                                                             \
                                                                                  \
    static void to_json(nlohmann::json &j, const type &t) {                       \
        t.toJson(j);                                                              \
    }                                                                             \
}

#endif //CHELPER_JSONUTIL_H
