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
        inline T fromJson(const nlohmann::json &json, const std::string &key) {
            return json.at(key).get<T>();
        }

        template<class T>
        std::optional<T> fromJsonOptionalLikely(const nlohmann::json &json, const std::string &key) {
            const auto &it = json.find(key);
            return HEDLEY_LIKELY(it != json.end()) ? std::optional(it->get<T>()) : std::nullopt;
        }

        template<class T>
        std::optional<T> fromJsonOptionalUnlikely(const nlohmann::json &json, const std::string &key) {
            const auto &it = json.find(key);
            return HEDLEY_UNLIKELY(it != json.end()) ? std::optional(it->get<T>()) : std::nullopt;
        }

        template<class T>
        inline void toJson(nlohmann::json &json, const std::string &key, const T &data) {
            json[key] = data;
        }

        template<class T>
        void toJsonOptionalLikely(nlohmann::json &json, const std::string &key, const std::optional<T> &data) {
            if (HEDLEY_LIKELY(data.has_value())) {
                json[key] = data.value();
            }
        }

        template<class T>
        void toJsonOptionalUnlikely(nlohmann::json &json, const std::string &key, const std::optional<T> &data) {
            if (HEDLEY_UNLIKELY(data.has_value())) {
                json[key] = data.value();
            }
        }

        nlohmann::json getJsonFromFile(const std::filesystem::path &path);

        nlohmann::json getBsonFromFile(const std::filesystem::path &path);

        void writeJsonToFile(const std::filesystem::path &path, const nlohmann::json &j);

        void writeBsonToFile(const std::filesystem::path &path, const nlohmann::json &j);

    } // JsonUtil

} // CHelper

#endif //CHELPER_JSONUTIL_H
