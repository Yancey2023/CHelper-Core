//
// Created by Yancey666 on 2023/11/7.
//

#ifndef CHELPER_CPACK_H
#define CHELPER_CPACK_H

#include <unordered_map>
#include <iostream>
#include "Manifest.h"
#include "id/BlockId.h"
#include "id/ItemId.h"

namespace CHelper {

    class CPack {
    public:
        Manifest manifest;
        std::unordered_map<std::string, std::vector<CHelper::NormalId>> normalIds;
        std::vector<CHelper::BlockId> blockIds;
        std::vector<CHelper::ItemId> itemIds;

        CPack(Manifest manifest,
              const std::unordered_map<std::string, std::vector<CHelper::NormalId>> &normalIds,
              const std::vector<CHelper::BlockId> &blockIds,
              const std::vector<CHelper::ItemId> &itemIds);

        static CPack getInstance(const std::filesystem::path& path);
    };

}

#endif //CHELPER_CPACK_H
