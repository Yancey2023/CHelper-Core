//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSONSTRING_H
#define CHELPER_NODEJSONSTRING_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeJsonString : public NodeBase {
    public:
        NodeJsonString(const std::optional<std::string> &id,
                       const std::optional<std::string> &description);

        NodeJsonString(const nlohmann::json &j,
                       [[maybe_unused]] const CPack &cpack);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEJSONSTRING_H
