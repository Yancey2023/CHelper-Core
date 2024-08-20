//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_JSONUTIL_H
#define CHELPER_JSONUTIL_H

#include <string>
#if CHelperOnlyReadBinary != true
#include <nlohmann/json.hpp>
#endif

namespace CHelper {

    class ErrorReason;

    namespace JsonUtil {

        class ConvertResult {
        public:
            std::u16string result;
            std::shared_ptr<ErrorReason> errorReason;
            std::vector<size_t> indexConvertList;
            bool isComplete = false;

            size_t convert(size_t index);
        };

        std::u16string string2jsonString(const std::u16string &input);

        ConvertResult jsonString2String(const std::u16string &input);

#if CHelperOnlyReadBinary != true

        template<class T>
        void encode(nlohmann::json &json, const std::string &key, const T &t) {
            json[key] = t;
        }

        template<class T>
        void encode(nlohmann::json &json, const std::string &key, const std::optional<T> &t) {
            if (HEDLEY_LIKELY(t.has_value())) {
                json[key] = t.value();
            }
        }

        template<class T>
        void decode(const nlohmann::json &json, const std::string &key, T &t) {
            json.at(key).get_to(t);
        }

        template<class T>
        void decode(const nlohmann::json &json, const std::string &key, std::optional<T> &t) {
            const auto &it = json.find(key);
            if (HEDLEY_LIKELY(it != json.end())) {
                t = std::make_optional<T>();
                it->get_to(t.value());
            } else {
                t = std::nullopt;
            }
        }

        template<class T>
        T read(const nlohmann::json &json, const std::string &key) {
            T t;
            decode(json, key, t);
            return t;
        }

        nlohmann::json getJsonFromFile(const std::filesystem::path &path);

        nlohmann::json getBsonFromFile(const std::filesystem::path &path);

        void writeJsonToFile(const std::filesystem::path &path, const nlohmann::json &j);

        void writeBsonToFile(const std::filesystem::path &path, const nlohmann::json &j);

#endif

    }// namespace JsonUtil


}// namespace CHelper

#if CHelperOnlyReadBinary != true

namespace nlohmann {

    template<typename T>
    struct [[maybe_unused]] adl_serializer<std::shared_ptr<T>> {

        static void from_json(const json &j, std::shared_ptr<T> &t) {
            t = std::make_shared<T>();
            j.get_to(*t);
        }

        static void to_json(json &j, const std::shared_ptr<T> &t) {
            j = *t;
        }
    };

    template<>
    struct [[maybe_unused]] adl_serializer<std::u16string> {

        static void from_json(const json &j, std::u16string &t) {
            t = utf8::utf8to16(j.get<std::string>());
        }

        static void to_json(json &j, const std::u16string &t) {
            j = utf8::utf16to8(t);
        }
    };

}// namespace nlohmann

#endif

#endif//CHELPER_JSONUTIL_H
