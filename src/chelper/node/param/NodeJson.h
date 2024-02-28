//
// Created by Yancey on 2024/2/28.
//

#ifndef CHELPER_NODEJSON_H
#define CHELPER_NODEJSON_H

#include "../NodeBase.h"

namespace CHelper::Node {

    //TODO 原始JSON文本的目标选择器要支持*，并且不支持

    class NodeJson : public NodeBase {
    public:
        const std::string key;
        const std::shared_ptr<NodeBase> nodeJson;

        NodeJson(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 std::string key,
                 std::shared_ptr<NodeBase> &nodeJson);

        NodeJson(const std::optional<std::string> &id,
                 const std::optional<std::string> &description,
                 const CPack &cpack,
                 const std::string &key);

        NodeJson(const nlohmann::json &j,
                 const CPack &cpack);

        [[nodiscard]] std::shared_ptr<NodeType> getNodeType() const override;

        void toJson(nlohmann::json &j) const override;

        ASTNode getASTNode(TokenReader &tokenReader) const override;

        void collectStructure(const ASTNode *astNode,
                              StructureBuilder &structure,
                              bool isMustHave) const override;

    };

} // CHelper::Node

#endif //CHELPER_NODEJSON_H
