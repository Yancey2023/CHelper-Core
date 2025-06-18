//
// Created by Yancey on 2024/2/21.
//

#pragma once

#ifndef CHELPER_SUGGESTION_H
#define CHELPER_SUGGESTION_H

#include "../resources/id/NormalId.h"
#include "TokensView.h"

namespace CHelper {

    class CHelperCore;

    class Suggestion {
    public:
        //要被替换的位置
        size_t start, end;
        //如果光标在最后面，是否需要在最后加空格
        bool isAddSpace;
        //内容
        std::shared_ptr<NormalId> content;

    public:
        Suggestion(size_t start, size_t end, bool isAddSpace, const std::shared_ptr<NormalId> &content);

        Suggestion(const TokensView &tokens, bool isAddSpace, const std::shared_ptr<NormalId> &content);

        [[nodiscard]] std::pair<std::u16string, size_t> apply(CHelperCore *core, const std::u16string_view &before) const;

        [[nodiscard]] XXH64_hash_t hashCode() const;
    };

}// namespace CHelper

#endif//CHELPER_SUGGESTION_H
