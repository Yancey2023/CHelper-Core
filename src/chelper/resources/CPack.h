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
        std::unordered_map<std::string, std::vector<std::shared_ptr<NormalId>>> normalIds;
        std::unordered_map<std::string, std::vector<std::shared_ptr<NamespaceId>>> namespaceIds;
        std::vector<std::shared_ptr<BlockId>> blockIds;
        std::vector<std::shared_ptr<ItemId>> itemIds;
        std::vector<std::shared_ptr<Node::NodePerCommand>> commands;
    private:
        explicit CPack(const std::filesystem::path &path);
    public:
        static CPack create(const std::filesystem::path &path);
    };

}

#endif //CHELPER_CPACK_H
