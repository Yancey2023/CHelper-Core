//
// Created by Yancey on 2023/11/8.
//

#ifndef CHELPER_ITEMID_H
#define CHELPER_ITEMID_H

#include "pch.h"
#include "NamespaceId.h"

namespace CHelper {

    namespace Node {

        class NodeBase;

    } // Node

    class ItemId : public NamespaceId {
    public:
        const std::optional<int> max;
        const std::optional<std::vector<std::string>> descriptions;
        const Node::NodeBase *nodeData;

        ItemId(const std::optional<std::string> &nameSpace,
               const std::string &name,
               const std::optional<std::string> &description,
               const std::optional<int> &max,
               const std::optional<std::vector<std::string>> &descriptions);

        explicit ItemId(const nlohmann::json &j);

        ~ItemId() override;

        ItemId(const ItemId &) = delete;

        ItemId &operator=(const ItemId &) = delete;

        void toJson(nlohmann::json &j) const override;
    };

} // CHelper

CREATE_ADL_SERIALIZER(CHelper::ItemId);

#endif //CHELPER_ITEMID_H
