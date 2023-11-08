//
// Created by Yancey666 on 2023/11/8.
//

#include <fstream>
#include "JsonUtil.h"

nlohmann::json CHelper::JsonUtil::getJsonFromPath(const std::filesystem::path& path){
    std::ifstream f(path);
    nlohmann::json j = nlohmann::json::parse(f);
    f.close();
    return j;
}