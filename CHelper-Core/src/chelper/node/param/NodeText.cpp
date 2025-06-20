//
// Created by Yancey on 2023/12/2.
//

#include <chelper/lexer/Lexer.h>
#include <chelper/node/NodeType.h>
#include <chelper/node/param/NodeText.h>

namespace CHelper::Node {

    NodeText::NodeText(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description,
                       const std::shared_ptr<NormalId> &data,
                       const std::function<ASTNode(const NodeBase *node, TokenReader &tokenReader)> &getTextASTNode)
        : NodeSerializable(id, description, false),
          data(data),
          getTextASTNode(getTextASTNode) {}

    void NodeText::init(const CPack &cpack) {
        getTextASTNode = [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
            return tokenReader.readUntilSpace(node);
        };
    }

    NodeTypeId::NodeTypeId NodeText::getNodeType() const {
        return NodeTypeId::TEXT;
    }

}// namespace CHelper::Node