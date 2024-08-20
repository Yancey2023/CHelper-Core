//
// Created by Yancey on 2023/11/13.
//

#include "NodeLF.h"

namespace CHelper::Node {

    NodeLF::NodeLF(const std::optional<std::u16string> &id,
                   const std::optional<std::u16string> &description)
        : NodeBase(id, description, false) {}

    NodeType *NodeLF::getNodeType() const {
        return NodeType::LF.get();
    }

    NodeLF *NodeLF::getInstance() {
        static std::unique_ptr<NodeLF> INSTANCE = std::make_unique<NodeLF>(u"LF", u"命令终止");
        return INSTANCE.get();
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

    void NodeLF::collectStructure(const ASTNode *astNode,
                                  StructureBuilder &structure,
                                  bool isMustHave) const {
        structure.append(u"\n");
    }

}// namespace CHelper::Node