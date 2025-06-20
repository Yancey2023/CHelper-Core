//
// Created by Yancey on 2023/11/12.
//

#pragma once

#ifndef CHELPER_NODENAMESPACEID_H
#define CHELPER_NODENAMESPACEID_H

#include "../../resources/CPack.h"
#include "../../resources/id/NamespaceId.h"
#include "../NodeBase.h"

namespace CHelper::Node {

    class NodeNamespaceId : public NodeSerializable {
    public:
        std::optional<std::string> key;
        std::optional<std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>> contents;
        std::optional<bool> ignoreError;
        std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> customContents;

        NodeNamespaceId() = default;

        NodeNamespaceId(const std::optional<std::string> &id,
                        const std::optional<std::u16string> &description,
                        const std::optional<std::string> &key,
                        bool ignoreError);

        void init(const CPack &cpack) override;

        [[nodiscard]] NodeTypeId::NodeTypeId getNodeType() const override;
    };

}// namespace CHelper::Node

CODEC_NODE(CHelper::Node::NodeNamespaceId, key, ignoreError, contents)

#endif//CHELPER_NODENAMESPACEID_H
