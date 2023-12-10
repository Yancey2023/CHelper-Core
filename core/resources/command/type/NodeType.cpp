//
// Created by Yancey666 on 2023/12/10.
//

#include "NodeType.h"

namespace CHelper::Node {

    std::vector<NodeType *> NodeType::NODE_TYPES;

    NodeType::NodeType(std::string nodeName,
                       std::shared_ptr<NodeBase>(*createNodeByJson)(const nlohmann::json &j, const CPack &cpack))
            : nodeName(std::move(nodeName)),
              createNodeByJson(createNodeByJson) {
        static int nextId = 0;
        NODE_TYPES.push_back(this);
        nodeId = nextId++;
    }

} // CHelper::Node