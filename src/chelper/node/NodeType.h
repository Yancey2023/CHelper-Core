//
// Created by Yancey on 2023/12/10.
//

#ifndef CHELPER_NODETYPE_H
#define CHELPER_NODETYPE_H

#include "pch.h"

namespace CHelper {

    class CPack;

    namespace Node {

        class NodeBase;

        class NodeType {
        public:
            std::string nodeName;

            std::shared_ptr<NodeBase> (*createNodeByJson)(const nlohmann::json &j, const CPack &cpack);

            NodeType(std::string nodeName,
                     std::shared_ptr<NodeBase>(*createNodeByJson)(const nlohmann::json &j, const CPack &cpack));

            template<class T>
            static NodeType create(const std::string &nodeName) {
                return NodeType(nodeName, [](const nlohmann::json &j, const CPack &cpack) -> std::shared_ptr<NodeBase> {
                    return std::make_shared<T>(j, cpack);
                });
            }

            static std::vector<const NodeType *> NODE_TYPES;

            static NodeType UNKNOWN, BLOCK, BOOLEAN, COMMAND, COMMAND_NAME, FLOAT, INTEGER, ITEM, LF, NAMESPACE_ID, NORMAL_ID, PER_COMMAND, POSITION, RELATIVE_FLOAT, STRING, TARGET_SELECTOR, TEXT;

            static void init();
        };

    } // Node

} // CHelper

#endif //CHELPER_NODETYPE_H
