//
// Created by Yancey on 2023/11/7.
//

#ifndef CHELPER_JSONUTIL_H
#define CHELPER_JSONUTIL_H

#include "pch.h"

// 把json文本转为对象
#define FROM_JSON(json, name, type...) json.at(#name).get<type>()

// 把json文本转为对象，数据不一定存在
#define FROM_JSON_OPTIONAL(json, name, type...) json.contains(#name) ? std::optional<type>(json.at(#name).get<type>()) : std::nullopt

// 把对象转json文本
#define TO_JSON(json, name) json.push_back({#name, name})

// 把象转json文本，数据不一定存在
#define TO_JSON_OPTIONAL(json, name)                                              \
if (name.has_value()) {                                                           \
    json.push_back({#name, name.value()});                                        \
}

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

namespace CHelper::JsonUtil {

    class ToJson {
    public:
        virtual void toJson(nlohmann::json &j) const = 0;
    };

    nlohmann::json getJsonFromPath(const std::filesystem::path &path);

}

#endif //CHELPER_JSONUTIL_H
