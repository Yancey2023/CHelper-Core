//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_MANIFEST_H
#define CHELPER_MANIFEST_H

#include "id/NormalId.h"
#include "pch.h"

namespace CHelper {

    class Manifest {
    public:
        std::optional<std::string> name, description, minecraftVersion, author, updateDate;
        std::string packId;
        int32_t versionCode;
        std::optional<bool> isBasicPack, isDefault;
    };

    CODEC_H(Manifest)

}// namespace CHelper

#endif//CHELPER_MANIFEST_H
