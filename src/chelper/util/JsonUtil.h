//
// Created by Yancey on 2023/11/7.
//

#pragma once

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

        template<class T>
        void encode(nlohmann::json &json, const std::string &key, const T &data) {
            json[key] = data;
        }

        template<class T>
        void encode(nlohmann::json &json, const std::string &key, const std::optional<T> &data) {
            if (HEDLEY_LIKELY(data.has_value())) {
                json[key] = data.value();
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

    }// namespace JsonUtil

}// namespace CHelper

namespace nlohmann {

    template<typename T>
    struct [[maybe_unused]] adl_serializer<std::shared_ptr<T>> {

        static void from_json(const json &j, std::shared_ptr<T> &t) {
            t = std::make_shared<T>(j);
        }

        static void to_json(json &j, const std::shared_ptr<T> &t) {
            j = *t;
        }
    };

}// namespace nlohmann

#endif//CHELPER_JSONUTIL_H
