//
// Created by Yancey on 2023/11/13.
//

#include <chelper/node/param/NodeLF.h>

namespace CHelper::Node {

    NodeLF::NodeLF(const std::optional<std::string> &id,
                   const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {}

    NodeTypeId::NodeTypeId NodeLF::getNodeType() const {
        return NodeTypeId::LF;
    }

    NodeWrapped *NodeLF::getInstance() {
        static std::unique_ptr<NodeLF> INSTANCE = std::make_unique<NodeLF>("LF", u"命令终止");
        static std::unique_ptr<NodeWrapped> INSTANCE_WRAPPED = std::make_unique<NodeWrapped>(INSTANCE.get());
        return INSTANCE_WRAPPED.get();
    }

    ASTNode NodeLF::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        tokenReader.skipToLF();
        TokensView tokens = tokenReader.collect();
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_UNLIKELY(tokens.hasValue())) {
            errorReason = ErrorReason::excess(tokens, u"命令后面有多余部分 -> " + std::u16string(tokens.toString()));
        }
        return ASTNode::simpleNode(this, tokens, errorReason);
    }

}// namespace CHelper::Node