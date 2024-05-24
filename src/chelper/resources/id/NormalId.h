//
// Created by Yancey on 2023/11/7.
//

#pragma once

#ifndef CHELPER_NORMALID_H
#define CHELPER_NORMALID_H

#include "pch.h"

namespace CHelper {

    class NormalId {
    public:
        std::string name;
        std::optional<std::string> description;

    private:
        bool isBuildHash = false;
        size_t nameHash = 0, mHashCode = 0;

    public:
        virtual ~NormalId() = default;

        void buildHash();

        [[nodiscard]] bool fastMatch(size_t strHash);

        [[nodiscard]] size_t hashCode();

        static std::shared_ptr<NormalId> make(const std::string &name, const std::optional<std::string> &description);
    };

    CODEC_H(NormalId)

}// namespace CHelper

#endif//CHELPER_NORMALID_H
