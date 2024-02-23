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
            int nodeId;
            std::string nodeName;

            std::shared_ptr<NodeBase> (*createNodeByJson)(const nlohmann::json &j, const CPack &cpack);

            NodeType(std::string nodeName,
                     std::shared_ptr<NodeBase>(*createNodeByJson)(const nlohmann::json &j, const CPack &cpack));

            static std::vector<NodeType *> NODE_TYPES;
        };

    } // Node

} // CHelper

#endif //CHELPER_NODETYPE_H
