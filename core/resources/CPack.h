//
// Created by Yancey666 on 2023/11/7.
//

#ifndef CHELPER_CPACK_H
#define CHELPER_CPACK_H

#include <pch.h>
#include "Manifest.h"
#include "id/BlockId.h"
#include "id/ItemId.h"
#include "command/Command.h"

namespace CHelper {

    class CPack {
    public:
        Manifest manifest;
        std::unordered_map<std::string, std::vector<NormalId>> normalIds;
        std::unordered_map<std::string, std::vector<NamespaceId>> namespaceIds;
        std::vector<BlockId> blockIds;
        std::vector<ItemId> itemIds;
        std::vector<CHelper::Command> commands;

        CPack(Manifest manifest);
    };

}

#endif //CHELPER_CPACK_H
