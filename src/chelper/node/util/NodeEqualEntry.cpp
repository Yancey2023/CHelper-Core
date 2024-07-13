//
// Created by Yancey on 2024-05-25.
//

#include "NodeEqualEntry.h"

#include "../param/NodeNormalId.h"
#include "../param/NodeText.h"
#include "NodeAny.h"
#include "NodeOr.h"

namespace CHelper::Node {

    static std::unique_ptr<NodeBase> nodeEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_EQUAL", "等于",
            NormalId::make("=", "等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                return tokenReader.readSymbolASTNode(node);
            });
    static std::unique_ptr<NodeBase> nodeNotEqual = std::make_unique<NodeText>(
            "TARGET_SELECTOR_ARGUMENT_NOT_EQUAL", "不等于",
            NormalId::make("=!", "不等于"),
            [](const NodeBase *node, TokenReader &tokenReader) -> ASTNode {
                tokenReader.push();
                auto childNodes = {tokenReader.readSymbolASTNode(node), tokenReader.readSymbolASTNode(node)};
                return ASTNode::andNode(node, childNodes, tokenReader.collect());
            });
    static std::unique_ptr<NodeBase> nodeEqualOrNotEqual = std::make_unique<NodeOr>(
            "TARGET_SELECTOR_ARGUMENT_SEPARATOR", "等于或不等于",
            std::vector<const NodeBase *>{
                    nodeEqual.get(), nodeNotEqual.get()},
            false);

    EqualData::EqualData(std::string name,
                         const std::optional<std::string> &description,
                         bool canUseNotEqual,
                         const NodeBase *nodeValue)
        : name(std::move(name)),
          description(description),
          canUseNotEqual(canUseNotEqual),
          nodeValue(nodeValue) {}

    NodeEqualEntry::NodeEqualEntry(const std::optional<std::string> &id,
                                   const std::optional<std::string> &description,
                                   std::vector<EqualData> equalDatas)
        : NodeBase(id, description, false),
          equalDatas(std::move(equalDatas)) {
        nodeKeyContent = std::make_shared<std::vector<std::shared_ptr<NormalId>>>();
        for (const auto &item: this->equalDatas) {
            nodeKeyContent->push_back(NormalId::make(item.name, item.description));
        }
        nodeKey = std::make_unique<NodeNormalId>(
                "KEY", "参数名",
                true, nodeKeyContent);
    }

    NodeType *NodeEqualEntry::getNodeType() const {
        return NodeType::EQUAL_ENTRY.get();
    }

    ASTNode NodeEqualEntry::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        tokenReader.push();
        std::vector<ASTNode> childNodes;
        // key
        ASTNode astNodeKey = getByChildNode(tokenReader, cpack, nodeKey.get(), "key");
        childNodes.push_back(astNodeKey);
        if (HEDLEY_UNLIKELY(astNodeKey.isError())) {
            return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
        }
        std::string key = astNodeKey.tokens.toString();
        auto it = std::find_if(equalDatas.begin(), equalDatas.end(), [&key](const auto &t) {
            return t.name == key;
        });
        // = or =!
        ASTNode astNodeSeparator = getByChildNode(
                tokenReader, cpack,
                it == equalDatas.end() || it->canUseNotEqual ? nodeEqualOrNotEqual.get() : nodeEqual.get(),
                "separator");
        childNodes.push_back(astNodeSeparator);
        if (HEDLEY_UNLIKELY(astNodeSeparator.isError())) {
            return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
        }
        //value
        if (HEDLEY_UNLIKELY(it == equalDatas.end())) {
            NodeAny::getNodeAny()->init(*cpack);
            childNodes.push_back(NodeAny::getNodeAny()->getASTNodeWithNextNode(tokenReader, cpack));
        } else {
            childNodes.push_back(it->nodeValue->getASTNodeWithNextNode(tokenReader, cpack));
        }
        return ASTNode::andNode(this, std::move(childNodes), tokenReader.collect());
    }

}// namespace CHelper::Node