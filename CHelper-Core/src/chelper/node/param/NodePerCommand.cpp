//
// Created by Yancey on 2023/11/10.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/param/NodePerCommand.h>

namespace CHelper::Node {

    void NodePerCommand::init(const CPack &cpack) {
        for (const auto &item: wrappedNodes) {
            Profile::push(R"(init node {}: "{}")", FORMAT_ARG(NodeTypeHelper::getName(item.getNodeType())), FORMAT_ARG(item.innerNode->id.value_or("UNKNOWN")));
            item.innerNode->init(cpack);
            Profile::pop();
        }
        for (const auto &item: wrappedNodes) {
            if (HEDLEY_UNLIKELY(item.nextNodes.empty())) {
                Profile::push("dismiss child node ids, the parent node is {} (in command {})",
                              FORMAT_ARG(item.innerNode->id.value_or("UNKNOWN")),
                              FORMAT_ARG(utf8::utf16to8(fmt::format(u"", StringUtil::join(name, u",")))));
                throw std::runtime_error("dismiss child node ids");
            }
        }
#ifdef CHelperDebug
        for (const auto &item: wrappedNodes) {
            bool flag1 = item.innerNode->getNodeType() == NodeTypeId::POSITION ||
                         item.innerNode->getNodeType() == NodeTypeId::RELATIVE_FLOAT;
            for (const auto &item2: item.nextNodes) {
                if (HEDLEY_UNLIKELY(item2 == NodeLF::getInstance())) {
                    continue;
                }
                bool flag2 = item2->innerNode->getNodeType() == NodeTypeId::POSITION ||
                             item2->innerNode->getNodeType() == NodeTypeId::RELATIVE_FLOAT;
                if (HEDLEY_UNLIKELY(flag1 && flag2 == item2->innerNode->isMustAfterSpace)) {
                    Profile::push(R"({} should be {} in node "{}")",
                                  "isMustAfterSpace",
                                  item2->innerNode->isMustAfterSpace ? "false" : "true",
                                  item2->innerNode->id.value_or("UNKNOWN"));
                    throw std::runtime_error("value is wrong");
                }
            }
        }
#endif
    }

    NodeTypeId::NodeTypeId NodePerCommand::getNodeType() const {
        return NodeTypeId::PER_COMMAND;
    }

    ASTNode NodePerCommand::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        std::vector<ASTNode> childASTNodes;
        childASTNodes.reserve(startNodes.size());
        for (const auto &item: startNodes) {
            tokenReader.push();
            DEBUG_GET_NODE_BEGIN(item)
            childASTNodes.push_back(item->getASTNodeWithIsMustAfterSpace(tokenReader, cpack, true));
            DEBUG_GET_NODE_END(item)
            tokenReader.restore();
        }
        tokenReader.push();
        tokenReader.skipToLF();
        return ASTNode::orNode(this, std::move(childASTNodes), tokenReader.collect());
    }

    std::optional<std::u16string> NodePerCommand::collectDescription(const ASTNode *node, size_t index) const {
        return std::nullopt;
    }

}// namespace CHelper::Node