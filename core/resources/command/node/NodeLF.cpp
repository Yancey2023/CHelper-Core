//
// Created by Yancey666 on 2023/11/13.
//

#include "NodeLF.h"

namespace CHelper::Node {

    NodeLF::NodeLF(const std::optional<std::string> &id,
                                  const std::optional<std::string> &description)
            : NodeBase(NodeType::EOL, id, description) {}

    std::shared_ptr<NodeLF> NodeLF::getInstance() {
        if (CHelper::Instance::NodeLF == nullptr) {
            CHelper::Instance::NodeLF = std::make_shared<NodeLF>(std::optional("LF"),  std::optional("Line Feet"));
        }
        return CHelper::Instance::NodeLF;
    }

} // CHelper::Node