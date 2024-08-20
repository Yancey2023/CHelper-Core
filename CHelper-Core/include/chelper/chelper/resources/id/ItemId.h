//
// Created by Yancey on 2023/11/8.
//

#pragma once

#ifndef CHELPER_ITEMID_H
#define CHELPER_ITEMID_H

#include "NamespaceId.h"
#include "pch.h"

namespace CHelper {

    namespace Node {

        class NodeBase;

    }// namespace Node

    class ItemId : public NamespaceId {
    public:
        std::optional<int32_t> max;
        std::optional<std::vector<std::u16string>> descriptions;

    private:
        std::vector<std::shared_ptr<Node::NodeBase>> nodeChildren;
        std::shared_ptr<Node::NodeBase> node = nullptr;

    public:
        std::shared_ptr<Node::NodeBase> getNode();
    };

    CODEC_WITH_PARENT_H(ItemId)

}// namespace CHelper

#endif//CHELPER_ITEMID_H
