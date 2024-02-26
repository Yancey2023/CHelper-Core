//
// Created by Yancey on 2023/11/7.
//

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
        std::shared_ptr<std::vector<std::shared_ptr<Node::NodeBase>>> commands = std::make_shared<std::vector<std::shared_ptr<Node::NodeBase>>>();
        //从这个节点开始检测
        std::shared_ptr<Node::NodeBase> mainNode;
    private:
        explicit CPack(const std::filesystem::path &path);

    public:
        static CPack create(const std::filesystem::path &path);

        std::shared_ptr<Node::NodeBase> getNormalId(const std::optional<std::string> &id,
                                                    const std::optional<std::string> &description,
                                                    const std::string &key) const;

        std::shared_ptr<Node::NodeBase> getNamespaceId(const std::optional<std::string> &id,
                                                       const std::optional<std::string> &description,
                                                       const std::string &key) const;

    };

}

#endif //CHELPER_CPACK_H
