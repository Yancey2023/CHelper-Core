//
// Created by Yancey on 2023/11/10.
//

#include <chelper/node/NodeType.h>
#include <chelper/node/param/NodePerCommand.h>

namespace CHelper::Node {

    void NodePerCommand::init(const CPack &cpack) {
        for (const auto &item: nodes) {
            if (HEDLEY_LIKELY(item->id.has_value())) {
                Profile::push(R"(init node {}: "{}")", FORMAT_ARG(utf8::utf16to8(NodeTypeHelper::getName(item->getNodeType()))), FORMAT_ARG(utf8::utf16to8(item->id.value())));
            } else {
                Profile::push("init node {}", FORMAT_ARG(utf8::utf16to8(NodeTypeHelper::getName(item->getNodeType()))));
            }
            item->init(cpack);
            Profile::pop();
        }
        for (const auto &item: nodes) {
            if (HEDLEY_UNLIKELY(item->nextNodes.empty())) {
                Profile::push("dismiss child node ids, the parent node is {} (in command {})",
                              FORMAT_ARG(item->id.has_value() ? utf8::utf16to8(item->id.value()) : "UNKNOWN"),
                              FORMAT_ARG(utf8::utf16to8(fmt::format(u"", StringUtil::join(name, u",")))));
                throw std::runtime_error("dismiss child node ids");
            }
        }
#ifdef CHelperDebug
        for (const auto &item: nodes) {
            bool flag1 = item->getNodeType() == NodeTypeId::POSITION ||
                         item->getNodeType() == NodeTypeId::RELATIVE_FLOAT;
            for (const auto &item2: item->nextNodes) {
                if (HEDLEY_UNLIKELY(item2 == NodeLF::getInstance())) {
                    continue;
                }
                bool flag2 = item2->getNodeType() == NodeTypeId::POSITION ||
                             item2->getNodeType() == NodeTypeId::RELATIVE_FLOAT;
                if (HEDLEY_UNLIKELY(flag1 && flag2 == item2->isMustAfterWhiteSpace)) {
                    Profile::push(R"({} should be {} in node "{}")",
                                  "isMustAfterWhiteSpace",
                                  item2->isMustAfterWhiteSpace ? "false" : "true",
                                  item2->id.has_value() ? utf8::utf16to8(item2->id.value()) : "UNKNOWN");
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
            childASTNodes.push_back(item->getASTNodeWithNextNode(
                    tokenReader, cpack, item->getNodeType() != NodeTypeId::REPEAT));
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