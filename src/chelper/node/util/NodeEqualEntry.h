//
// Created by Yancey on 2024-05-25.
//

#ifndef CHELPER_NODEEQUALENTRY_H
#define CHELPER_NODEEQUALENTRY_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class EqualData {
    public:
        std::wstring name;
        std::optional<std::wstring> description;
        bool canUseNotEqual;
        const NodeBase *nodeValue;

        EqualData(std::wstring name,
                  const std::optional<std::wstring> &description,
                  bool canUseNotEqual,
                  const NodeBase *nodeValue);
    };

    class NodeEqualEntry : public NodeBase {
    public:
        std::vector<EqualData> equalDatas;

    private:
        std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> nodeKeyContent;
        std::unique_ptr<NodeBase> nodeKey;

    public:
        NodeEqualEntry() = default;

        NodeEqualEntry(const std::optional<std::wstring> &id,
                       const std::optional<std::wstring> &description,
                       std::vector<EqualData> equalDatas);

        [[nodiscard]] NodeType *getNodeType() const override;

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEEQUALENTRY_H
