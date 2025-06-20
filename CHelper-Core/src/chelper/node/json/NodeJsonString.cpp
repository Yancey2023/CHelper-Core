//
// Created by Yancey on 2024/2/28.
//

#include <chelper/lexer/Lexer.h>
#include <chelper/node/NodeType.h>
#include <chelper/node/json/NodeJsonString.h>
#include <chelper/node/util/NodeOr.h>
#include <chelper/parser/Parser.h>

namespace CHelper::Node {

    static std::shared_ptr<NormalId> doubleQuoteMask = NormalId::make(u"\"", u"双引号");

    NodeJsonString::NodeJsonString(const std::optional<std::string> &id,
                                   const std::optional<std::u16string> &description)
        : NodeSerializable(id, description, false) {
        nodeData = std::make_unique<NodeOr>(std::vector<const NodeBase *>(), false);
    }

    void NodeJsonString::init(const CPack &cpack) {
        if (HEDLEY_UNLIKELY(data.has_value())) {
            for (const auto &item: data.value()) {
                item->init(cpack);
            }
        }
        std::vector<const NodeBase *> nodeDataElement;
        if (HEDLEY_UNLIKELY(data.has_value())) {
            nodeDataElement.reserve(data.value().size());
            for (const auto &item: data.value()) {
                nodeDataElement.push_back(item.get());
            }
        }
        nodeData = std::make_unique<NodeOr>(std::move(nodeDataElement), false);
    }

    NodeTypeId::NodeTypeId NodeJsonString::getNodeType() const {
        return NodeTypeId::JSON_STRING;
    }

}// namespace CHelper::Node