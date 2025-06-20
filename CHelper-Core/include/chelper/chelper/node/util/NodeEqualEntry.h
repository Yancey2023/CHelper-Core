//
// Created by Yancey on 2024-05-25.
//

#ifndef CHELPER_NODEEQUALENTRY_H
#define CHELPER_NODEEQUALENTRY_H

#include "../NodeBase.h"

namespace CHelper::Node {

    class EqualData {
    public:
        std::u16string name;
        std::optional<std::u16string> description;
        bool canUseNotEqual;
        const NodeBase *nodeValue;

        EqualData(std::u16string name,
                  const std::optional<std::u16string> &description,
                  bool canUseNotEqual,
                  const NodeBase *nodeValue);
    };

    class NodeEqualEntry : public NodeBase {
    public:
        static std::unique_ptr<NodeBase> nodeEqual;
        static std::unique_ptr<NodeBase> nodeNotEqual;
        static std::unique_ptr<NodeBase> nodeEqualOrNotEqual;
        std::vector<EqualData> equalDatas;
        std::shared_ptr<std::vector<std::shared_ptr<NormalId>>> nodeKeyContent;
        std::unique_ptr<NodeBase> nodeKey;

        NodeEqualEntry() = default;

        explicit NodeEqualEntry(std::vector<EqualData> equalDatas);

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;
    };

}// namespace CHelper::Node

#endif//CHELPER_NODEEQUALENTRY_H
