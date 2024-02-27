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

            static NodeType UNKNOWN;
            static NodeType BLOCK;
            static NodeType BOOLEAN;
            static NodeType COMMAND;
            static NodeType COMMAND_NAME;
            static NodeType FLOAT;
            static NodeType INTEGER;
            static NodeType ITEM;
            static NodeType LF;
            static NodeType NAMESPACE_ID;
            static NodeType NORMAL_ID;
            static NodeType PER_COMMAND;
            static NodeType POSITION;
            static NodeType RELATIVE_FLOAT;
            static NodeType STRING;
            static NodeType TARGET_SELECTOR;
            static NodeType TEXT;
            static NodeType RANGE;

            static void init();
        };

    } // Node

} // CHelper

#endif //CHELPER_NODETYPE_H
