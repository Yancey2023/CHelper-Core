//
// Created by Yancey on 2024/2/28.
//

#include <chelper/node/param/NodeJson.h>
#include <chelper/resources/CPack.h>

namespace CHelper::Node {

    NodeJson::NodeJson(const std::optional<std::string> &id,
                       const std::optional<std::u16string> &description,
                       std::string key)
        : NodeSerializable(id, description, false),
          key(std::move(key)) {}

    void NodeJson::init(const CPack &cpack) {
        for (const auto &item: cpack.jsonNodes) {
            if (HEDLEY_UNLIKELY(item->id == key)) {
                nodeJson = item.get();
                return;
            }
        }
        Profile::push("linking contents to {}", FORMAT_ARG(key));
        Profile::push("failed to find json data in the cpack -> {}", FORMAT_ARG(key));
        throw std::runtime_error("failed to find json data");
    }

    NodeTypeId::NodeTypeId NodeJson::getNodeType() const {
        return NodeTypeId::JSON;
    }

    ASTNode NodeJson::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, nodeJson);
    }

    void NodeJson::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
        structure.append(isMustHave, description.value_or(u"JSON文本"));
    }

}// namespace CHelper::Node