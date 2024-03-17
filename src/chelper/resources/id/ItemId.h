//
// Created by Yancey on 2023/11/8.
//

#pragma once

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

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<CHelper::ItemId> {

    static CHelper::ItemId from_json(const nlohmann::json &j) {
        return CHelper::ItemId(j);
    }

    static void to_json(nlohmann::json &j, const CHelper::ItemId &t) {
        t.toJson(j);
    }

};

template<>
struct [[maybe_unused]] nlohmann::adl_serializer<std::shared_ptr<CHelper::ItemId>> {

    static std::shared_ptr<CHelper::ItemId> from_json(const nlohmann::json &j) {
        return std::make_shared<CHelper::ItemId>(j);
    }

    static void to_json(nlohmann::json &j, const std::shared_ptr<CHelper::ItemId> &t) {
        t->toJson(j);
    }

};

#endif //CHELPER_ITEMID_H
