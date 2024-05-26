//
// Created by Yancey on 2023/11/11.
//

#pragma once

#ifndef CHELPER_NAMESPACEID_H
#define CHELPER_NAMESPACEID_H

#include "NormalId.h"
#include "pch.h"

namespace CHelper {

    class NamespaceId : public NormalId {
    public:
        std::optional<std::string> idNamespace;

    private:
        std::shared_ptr<NormalId> idWithNamespace;

    public:
        std::shared_ptr<NormalId> &getIdWithNamespace();
    };

    CODEC_WITH_PARENT_H(NamespaceId)

}// namespace CHelper

#endif//CHELPER_NAMESPACEID_H
