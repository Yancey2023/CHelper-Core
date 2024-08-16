//
// Created by Yancey on 2023/11/13.
//

#include "NodeLF.h"

namespace CHelper::Node {

    NodeLF::NodeLF(const std::optional<std::wstring> &id,
                   const std::optional<std::wstring> &description)
        : NodeBase(id, description, false) {}

    NodeType *NodeLF::getNodeType() const {
        return NodeType::LF.get();
    }

    NodeLF *NodeLF::getInstance() {
        static std::unique_ptr<NodeLF> INSTANCE = std::make_unique<NodeLF>(L"LF", L"命令终止");
        return INSTANCE.get();
    }

    ASTNode NodeLF::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        tokenReader.skipToLF();
        TokensView tokens = tokenReader.collect();
        std::shared_ptr<ErrorReason> errorReason;
        if (HEDLEY_UNLIKELY(tokens.hasValue())) {
            errorReason = ErrorReason::excess(tokens, L"命令后面有多余部分 -> " + std::wstring(tokens.toString()));
        }
        return ASTNode::simpleNode(this, tokens, errorReason);
    }

    void NodeLF::collectStructure(const ASTNode *astNode,
                                  StructureBuilder &structure,
                                  bool isMustHave) const {
        structure.append(L"\n");
    }

}// namespace CHelper::Node