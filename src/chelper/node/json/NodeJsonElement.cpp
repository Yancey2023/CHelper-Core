//
// Created by Yancey on 2024/2/28.
//

#include "NodeJsonElement.h"
#include "../param/NodeLF.h"
#include "NodeJsonObject.h"
#include "NodeJsonList.h"

namespace CHelper::Node {

    NodeJsonElement::NodeJsonElement(const std::optional<std::string> &id,
                                     const std::optional<std::string> &description,
                                     const std::vector<std::shared_ptr<NodeBase>> &nodes)
            : NodeBase(id, description, false),
              nodes(nodes) {}

    NodeJsonElement::NodeJsonElement(const nlohmann::json &j,
                                     [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, false) {
        if (!id.has_value()) {
            Profile::push("dismiss json data id");
            throw Exception::NodeLoadFailed();
        }
        Profile::push(ColorStringBuilder().red("loading nodes").build());
        for (const auto &item: j.at("node")) {
            nodes.push_back(getNodeFromJson(item, cpack));
        }
        Profile::next(ColorStringBuilder().red("loading start nodes").build());
        auto startNodeId = FROM_JSON(j, start, std::string);
        Profile::next(ColorStringBuilder()
                              .red("linking startNode \"")
                              .purple(startNodeId)
                              .red("\" to nodes")
                              .build());
        if (startNodeId == "LF") {
            start = NodeLF::getInstance();
        } else {
            for (auto &node: nodes) {
                if (node->id == startNodeId) {
                    start = node;
                    break;
                }
            }
        }
        if (start == nullptr) {
            throw Exception::UnknownNodeId(startNodeId, id.value());
        }
        for (const auto &item: nodes) {
            if (item->getNodeType() == NodeType::JSON_LIST) {
                std::static_pointer_cast<NodeJsonList>(item)->init(nodes);
            } else if (item->getNodeType() == NodeType::JSON_OBJECT) {
                std::static_pointer_cast<NodeJsonObject>(item)->init(nodes);
            }
        }
        Profile::pop();
    }

    NodeJsonElement::~NodeJsonElement() {
        for (const auto &item: nodes) {
            if (item->getNodeType() == NodeType::JSON_LIST) {
                if (item != nullptr) {
                    std::static_pointer_cast<NodeJsonList>(item) = nullptr;
                }
            } else if (item->getNodeType() == NodeType::JSON_OBJECT) {
                for (const auto &item2: *std::static_pointer_cast<NodeJsonObject>(item)->data) {
                    if (item2 != nullptr) {
                        std::static_pointer_cast<NodeJsonEntry>(item2)->nodeEntry = nullptr;
                    }
                }
            }
        }
    }

    void NodeJsonElement::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        j["node"] = nodes;
        j["start"] = start->id.value();
    }

    ASTNode NodeJsonElement::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getByChildNode(tokenReader, cpack, start);
    }

} // CHelper::Node