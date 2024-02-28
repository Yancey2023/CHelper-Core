//
// Created by Yancey on 2023/11/8.
//

#include "JsonUtil.h"

nlohmann::json CHelper::JsonUtil::getJsonFromPath(const std::filesystem::path &path) {
    Profile::push("reading and parsing json data in path: " + path.string());
    std::ifstream f(path);
    nlohmann::json j = nlohmann::json::parse(f);
    f.close();
    Profile::pop();
    return j;
}