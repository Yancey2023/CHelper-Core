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
            std::function<std::shared_ptr<NodeBase>(const nlohmann::json &j, const CPack &cpack)> createNodeByJson;

            NodeType(std::string nodeName,
                     std::function<std::shared_ptr<NodeBase>(const nlohmann::json &j,
                                                             const CPack &cpack)> createNodeByJson);

            static std::vector<std::shared_ptr<NodeType>> NODE_TYPES;

            static bool canLoadNodeJson;

            static std::shared_ptr<NodeType> UNKNOWN;
            static std::shared_ptr<NodeType> BLOCK;
            static std::shared_ptr<NodeType> BOOLEAN;
            static std::shared_ptr<NodeType> COMMAND;
            static std::shared_ptr<NodeType> COMMAND_NAME;
            static std::shared_ptr<NodeType> FLOAT;
            static std::shared_ptr<NodeType> INTEGER;
            static std::shared_ptr<NodeType> ITEM;
            static std::shared_ptr<NodeType> LF;
            static std::shared_ptr<NodeType> NAMESPACE_ID;
            static std::shared_ptr<NodeType> NORMAL_ID;
            static std::shared_ptr<NodeType> PER_COMMAND;
            static std::shared_ptr<NodeType> POSITION;
            static std::shared_ptr<NodeType> RELATIVE_FLOAT;
            static std::shared_ptr<NodeType> STRING;
            static std::shared_ptr<NodeType> TARGET_SELECTOR;
            static std::shared_ptr<NodeType> TEXT;
            static std::shared_ptr<NodeType> RANGE;
            static std::shared_ptr<NodeType> JSON;
            static std::shared_ptr<NodeType> JSON_OBJECT;
            static std::shared_ptr<NodeType> JSON_LIST;
            static std::shared_ptr<NodeType> JSON_STRING;
            static std::shared_ptr<NodeType> JSON_INTEGER;
            static std::shared_ptr<NodeType> JSON_FLOAT;
            static std::shared_ptr<NodeType> JSON_BOOLEAN;
            static std::shared_ptr<NodeType> JSON_NULL;

            static void init();
        };

    } // Node

} // CHelper

#endif //CHELPER_NODETYPE_H
