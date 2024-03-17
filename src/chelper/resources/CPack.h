//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_CPACK_H
#define CHELPER_CPACK_H

#include "pch.h"
#include "Manifest.h"
#include "id/BlockId.h"
#include "id/ItemId.h"
#include "../node/param/NodePerCommand.h"

namespace CHelper {

    class CPack {
    public:
        Manifest manifest;
        std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>> normalIds;
        std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>> namespaceIds;
        std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> blockIds = std::make_shared<std::vector<std::shared_ptr<NamespaceId>>>();
        std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> itemIds = std::make_shared<std::vector<std::shared_ptr<NamespaceId>>>();
        std::vector<std::unique_ptr<Node::NodeBase>> jsonNodes;
        std::vector<std::pair<Node::NodeBase *, Node::NodeBase *>> repeatNodes;
        std::shared_ptr<std::vector<std::unique_ptr<Node::NodeBase>>> commands = std::make_shared<std::vector<std::unique_ptr<Node::NodeBase>>>();
        //从这个节点开始检测
        Node::NodeBase *mainNode = nullptr;
    private:
        std::vector<std::unique_ptr<Node::NodeBase>> repeatNodeData;

    public:
        explicit CPack(const std::filesystem::path &path);

        explicit CPack(const nlohmann::json &j);

        ~CPack();

        CPack(const CPack &) = delete;

        CPack &operator=(const CPack &) = delete;

    private:
        void applyId(const nlohmann::json &j);

        void applyJson(const nlohmann::json &j);

        void applyRepeat(const nlohmann::json &j);

        void applyCommand(const nlohmann::json &j);

        void afterApply();

    public:
        static std::unique_ptr<CPack> createByDirectory(const std::filesystem::path &path);

        static std::unique_ptr<CPack> createByJson(const nlohmann::json &j);

        void writeJsonToDirectory(const std::filesystem::path &path) const;

        [[nodiscard]] nlohmann::json toJson() const;

        void writeJsonToFile(const std::filesystem::path &path) const;

        void writeBsonToFile(const std::filesystem::path &path) const;

        [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>
        getNormalId(const std::string &key) const;

        [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>
        getNamespaceId(const std::string &key) const;

    };

}

#endif //CHELPER_CPACK_H
