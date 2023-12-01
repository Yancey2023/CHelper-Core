//
// Created by Yancey666 on 2023/11/30.
//

#ifndef CHELPER_CPACKUTIL_H
#define CHELPER_CPACKUTIL_H

#include "pch.h"
#include "../resources/CPack.h"

namespace CHelper {

    namespace Node {

        std::shared_ptr<NodeBase> getNodeFromJson(const nlohmann::json &j,
                                                  const CPack &cpack);
    } // Node

    Command getCommandFromJson(const nlohmann::json &j,
                               const CPack &cpack);

    CPack getCPack(const std::filesystem::path &path);

} // CHelper

#endif //CHELPER_CPACKUTIL_H
