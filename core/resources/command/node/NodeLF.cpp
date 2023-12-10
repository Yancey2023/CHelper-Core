//
// Created by Yancey666 on 2023/11/13.
//

#include "NodeLF.h"

namespace CHelper::Node {

    NodeType NodeLF::TYPE("LF", [](const nlohmann::json &j, const CPack &cpack) {
        return (std::shared_ptr<NodeBase>) NodeLF::getInstance();
    });

    NodeType NodeLF::getNodeType() const { return NodeLF::TYPE; }


    NodeLF::NodeLF(const std::optional<std::string> &id,
                   const std::optional<std::string> &description)
            : NodeBase(id, description) {}

    std::shared_ptr<NodeLF> NodeLF::getInstance() {
        static std::shared_ptr<Node::NodeLF> INSTANCE;
        if (INSTANCE == nullptr) {
            INSTANCE = std::make_shared<NodeLF>(std::optional("LF"), std::optional("Line Feet"));
        }
        return INSTANCE;
    }

} // CHelper::Node