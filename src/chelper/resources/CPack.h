//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_CPACK_H
#define CHELPER_CPACK_H

#include "../node/json/NodeJsonElement.h"
#include "../node/param/NodeCommand.h"
#include "../node/param/NodePerCommand.h"
#include "Manifest.h"
#include "id/BlockId.h"
#include "id/ItemId.h"
#include "pch.h"

namespace CHelper {

    class RepeatData {
    public:
        std::string id;
        std::vector<std::unique_ptr<Node::NodeBase>> breakNodes;
        std::vector<std::vector<std::unique_ptr<Node::NodeBase>>> repeatNodes;
    };

    CODEC_H(RepeatData)

    class CPack {
    public:
        Manifest manifest;
        std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>> normalIds;
        std::unordered_map<std::string, std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>> namespaceIds;
        std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> blockIds = std::make_shared<std::vector<std::shared_ptr<NamespaceId>>>();
        std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>> itemIds = std::make_shared<std::vector<std::shared_ptr<NamespaceId>>>();
        std::vector<std::unique_ptr<Node::NodeJsonElement>> jsonNodes;
        std::vector<RepeatData> repeatNodeData;
        std::vector<std::pair<Node::NodeBase *, Node::NodeBase *>> repeatNodes;
        std::shared_ptr<std::vector<std::unique_ptr<Node::NodePerCommand>>> commands = std::make_shared<std::vector<std::unique_ptr<Node::NodePerCommand>>>();
        std::unique_ptr<Node::NodeCommand> mainNode;

    private:
        std::vector<std::unique_ptr<Node::NodeBase>> repeatCacheNodes;

    public:
        explicit CPack(const std::filesystem::path &path);

        explicit CPack(const nlohmann::json &j);

        explicit CPack(BinaryReader &binaryReader);

    private:
        void applyId(const nlohmann::json &j);

        void applyJson(const nlohmann::json &j);

        void applyRepeat(const nlohmann::json &j);

        void applyCommand(const nlohmann::json &j);

        void afterApply();

    public:
        static std::unique_ptr<CPack> createByDirectory(const std::filesystem::path &path);

        static std::unique_ptr<CPack> createByJson(const nlohmann::json &j);

        static std::unique_ptr<CPack> createByBinary(BinaryReader &binaryReader);

        void writeJsonToDirectory(const std::filesystem::path &path) const;

        [[nodiscard]] nlohmann::json toJson() const;

        void writeJsonToFile(const std::filesystem::path &path) const;

        void writeBsonToFile(const std::filesystem::path &path) const;

        void writeBinToFile(const std::filesystem::path &path) const;

        [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<NormalId>>>
        getNormalId(const std::string &key) const;

        [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<NamespaceId>>>
        getNamespaceId(const std::string &key) const;
    };

}// namespace CHelper

#endif//CHELPER_CPACK_H
