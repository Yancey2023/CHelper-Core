//
// Created by Yancey on 2024/2/21.
//

#pragma once

#ifndef CHELPER_SUGGESTION_H
#define CHELPER_SUGGESTION_H

#include "../lexer/Token.h"
#include "../resources/id/NormalId.h"
#include "TokensView.h"

namespace CHelper {

    class Core;

    class Suggestion {
    public:
        //要被替换的位置
        size_t start, end;
        //如果光标在最后面，是否需要在最后加空格
        bool isAddWhitespace;
        //内容
        std::shared_ptr<NormalId> content;

    private:
        const size_t mHashCode;

    public:
        Suggestion(size_t start, size_t end, bool isAddWhitespace, const std::shared_ptr<NormalId> &content);

        Suggestion(const TokensView &tokens, bool isAddWhitespace, const std::shared_ptr<NormalId> &content);

        [[nodiscard]] std::string apply(Core *core, const std::string_view &before) const;

        [[nodiscard]] size_t hashCode() const {
            return mHashCode;
        }

        [[nodiscard]] bool equal(const Suggestion &suggestion) const {
            return mHashCode == suggestion.mHashCode;
        }
    };

}// namespace CHelper

#endif//CHELPER_SUGGESTION_H
