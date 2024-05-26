//
// Created by Yancey on 2024-05-25.
//

#ifndef CHELPER_NODEEQUALENTRY_H
#define CHELPER_NODEEQUALENTRY_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class EqualData {
    public:
        std::string name;
        std::optional<std::string> description;
        bool canUseNotEqual;
        const NodeBase *nodeValue;

        EqualData(std::string name,
                  const std::optional<std::string> &description,
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
        NodeEqualEntry(const std::optional<std::string> &id,
                       const std::optional<std::string> &description,
                       std::vector<EqualData> equalDatas);

        ASTNode getASTNode(TokenReader &tokenReader, const CPack *cpack) const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEEQUALENTRY_H
