//
// Created by Yancey on 2024/2/28.
//

#include "NodeJson.h"
#include "../../resources/CPack.h"

namespace CHelper::Node {

    NodeJson::NodeJson(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       std::string key,
                       std::shared_ptr<NodeBase> &nodeJson)
            : NodeBase(id, description, false),
              key(std::move(key)),
              nodeJson(nodeJson) {}

    static std::shared_ptr<NodeBase> getNodeJsonFromCPack(const CPack &cpack,
                                                          const std::string &key) {
        for (const auto &item: cpack.jsonNodes) {
            if (item->id == key) {
                return item;
            }
        }
        Profile::push(ColorStringBuilder()
                              .red("linking contents to ")
                              .purple(key)
                              .build());
        Profile::push(ColorStringBuilder()
                              .red("failed to find json data in the cpack")
                              .normal(" -> ")
                              .purple(key)
                              .build());
        throw Exception::NodeLoadFailed();
    }

    NodeJson::NodeJson(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       const CPack &cpack,
                       const std::string &key)
            : NodeBase(id, description, false),
              key(key),
              nodeJson(getNodeJsonFromCPack(cpack, key)) {}

    NodeJson::NodeJson(const nlohmann::json &j,
                       [[maybe_unused]] const CPack &cpack)
            : NodeBase(j, true),
              key(FROM_JSON(j, key, std::string)),
              nodeJson(getNodeJsonFromCPack(cpack, key)) {}

    std::shared_ptr<NodeType> NodeJson::getNodeType() const {
        return NodeType::JSON;
    }

    void NodeJson::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        TO_JSON(j, key);
    }

    ASTNode NodeJson::getASTNode(TokenReader &tokenReader, const CPack &cpack) const {
        return getByChildNode(tokenReader, cpack, nodeJson);
    }

    void NodeJson::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
        structure.append(isMustHave, description.value_or("JSON文本"));
    }

} // CHelper::Node