//
// Created by Yancey on 2024/2/28.
//

#include "NodeJson.h"

#include "../../resources/CPack.h"

namespace CHelper::Node {

    NodeJson::NodeJson(const std::optional<std::wstring> &id,
                       const std::optional<std::wstring> &description,
                       std::wstring key)
        : NodeBase(id, description, false),
          key(std::move(key)) {}

    void NodeJson::init(const CPack &cpack) {
        for (const auto &item: cpack.jsonNodes) {
            if (HEDLEY_UNLIKELY(item->id == key)) {
                nodeJson = item.get();
                return;
            }
        }
        Profile::push("linking contents to {}", key);
        Profile::push("failed to find json data in the cpack -> {}", key);
        throw std::runtime_error("failed to find json data");
    }

    NodeType *NodeJson::getNodeType() const {
        return NodeType::JSON.get();
    }

    ASTNode NodeJson::getASTNode(TokenReader &tokenReader, const CPack *cpack) const {
        return getByChildNode(tokenReader, cpack, nodeJson);
    }

    void NodeJson::collectStructure(const ASTNode *astNode,
                                    StructureBuilder &structure,
                                    bool isMustHave) const {
        structure.append(isMustHave, description.value_or(L"JSON文本"));
    }

    CODEC_NODE(NodeJson, key)

}// namespace CHelper::Node