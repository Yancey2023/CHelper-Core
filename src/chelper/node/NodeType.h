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
            std::function<std::unique_ptr<NodeBase>(const nlohmann::json &j, const CPack &cpack)> createNodeByJson;

            NodeType(std::string nodeName,
                     std::function<std::unique_ptr<NodeBase>(const nlohmann::json &j,
                                                             const CPack &cpack)> createNodeByJson);

            static std::vector<NodeType*> NODE_TYPES;

            static bool canLoadNodeJson;

            static std::unique_ptr<NodeType> UNKNOWN;
            static std::unique_ptr<NodeType> BLOCK;
            static std::unique_ptr<NodeType> BOOLEAN;
            static std::unique_ptr<NodeType> COMMAND;
            static std::unique_ptr<NodeType> COMMAND_NAME;
            static std::unique_ptr<NodeType> FLOAT;
            static std::unique_ptr<NodeType> INTEGER;
            static std::unique_ptr<NodeType> ITEM;
            static std::unique_ptr<NodeType> LF;
            static std::unique_ptr<NodeType> NAMESPACE_ID;
            static std::unique_ptr<NodeType> NORMAL_ID;
            static std::unique_ptr<NodeType> PER_COMMAND;
            static std::unique_ptr<NodeType> POSITION;
            static std::unique_ptr<NodeType> RELATIVE_FLOAT;
            static std::unique_ptr<NodeType> REPEAT;
            static std::unique_ptr<NodeType> STRING;
            static std::unique_ptr<NodeType> TARGET_SELECTOR;
            static std::unique_ptr<NodeType> TEXT;
            static std::unique_ptr<NodeType> RANGE;
            static std::unique_ptr<NodeType> XP_INTEGER;
            static std::unique_ptr<NodeType> JSON;
            static std::unique_ptr<NodeType> JSON_OBJECT;
            static std::unique_ptr<NodeType> JSON_LIST;
            static std::unique_ptr<NodeType> JSON_STRING;
            static std::unique_ptr<NodeType> JSON_INTEGER;
            static std::unique_ptr<NodeType> JSON_FLOAT;
            static std::unique_ptr<NodeType> JSON_BOOLEAN;
            static std::unique_ptr<NodeType> JSON_NULL;

            static void init();
        };

    } // Node

} // CHelper

#endif //CHELPER_NODETYPE_H
