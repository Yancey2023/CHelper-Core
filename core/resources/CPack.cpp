//
// Created by Yancey666 on 2023/11/7.
//

#include "CPack.h"
#include "../util/JsonUtil.h"

CHelper::CPack::CPack(Manifest manifest,
                      const std::unordered_map<std::string, std::vector<CHelper::NormalId>> &normalIds,
                      const std::vector<CHelper::BlockId> &blockIds,
                      const std::vector<CHelper::ItemId> &itemIds)
        : manifest(std::move(manifest)),
          normalIds(normalIds),
          blockIds(blockIds),
          itemIds(itemIds) {}

CHelper::CPack CHelper::CPack::getInstance(const std::filesystem::path& path) {
    Manifest manifest = JsonUtil::getJsonFromPath(path / "manifest.json").get<Manifest>();
    std::unordered_map<std::string, std::vector<CHelper::NormalId>> normalIds;
    std::vector<CHelper::BlockId> blockIds;
    std::vector<CHelper::ItemId> itemIds;
    for (const auto &idFile: path / "id") {
        nlohmann::json j = JsonUtil::getJsonFromPath(idFile);
        std::string type = j.at("type").get<std::string>();
        if (type == "normal") {
            normalIds.emplace(
                    FROM_JSON(j, id, std::string),
                    FROM_JSON(j, content, std::vector<CHelper::NormalId>)
            );
        } else if (type == "block") {
            blockIds = FROM_JSON(j, blocks, std::vector<CHelper::BlockId>);
        } else if (type == "item") {
            itemIds = FROM_JSON(j, items, std::vector<CHelper::ItemId>);
        } else {
            std::cout << idFile << "使用了不存在的ID类型 -> " << type << std::endl;
        }
    }
    return {manifest, normalIds, blockIds, itemIds};
}