//
// Created by Yancey on 2024/2/28.
//

#include "NodeJson.h"
#include "../../resources/CPack.h"

namespace CHelper::Node {

    NodeJson::NodeJson(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       std::string key,
                       NodeBase *nodeJson)
            : NodeBase(id, description, false),
              key(std::move(key)),
              nodeJson(nodeJson) {}

    static NodeBase *getNodeJsonFromCPack(const CPack &cpack,
                                          const std::string &key) {
        for (const auto &item: cpack.jsonNodes) {
            if (item->id == key) {
                return item.get();
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
              key(JsonUtil::fromJson<std::string>(j, "key")),
              nodeJson(getNodeJsonFromCPack(cpack, key)) {}

    NodeType *NodeJson::getNodeType() const {
        return NodeType::JSON.get();
    }

    void NodeJson::toJson(nlohmann::json &j) const {
        NodeBase::toJson(j);
        JsonUtil::toJson(j, "key", key);
    }

    ASTNode NodeJson::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, nodeJson);
    }

    void NodeJson::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
        structure.append(isMustHave, description.value_or("JSON文本"));
    }

} // CHelper::Node