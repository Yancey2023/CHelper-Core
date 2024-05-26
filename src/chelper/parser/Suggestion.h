//
// Created by Yancey on 2024/2/21.
//

#pragma once

#ifndef CHELPER_SUGGESTION_H
#define CHELPER_SUGGESTION_H

#include "../lexer/Token.h"
#include "../resources/id/NormalId.h"
#include "../util/VectorView.h"

namespace CHelper {

    class Core;

    class Suggestion {
    public:
        //TODO 为代码补全建议添加优先级
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

        Suggestion(const VectorView<Token> &tokens, bool isAddWhitespace, const std::shared_ptr<NormalId> &content);

        [[nodiscard]] std::string onClick(Core *core, const std::string &before) const;

        [[nodiscard]] size_t hashCode() const {
            return mHashCode;
        }

        [[nodiscard]] bool equal(const Suggestion &suggestion) const {
            return mHashCode == suggestion.mHashCode;
        }
    };

}// namespace CHelper

#endif//CHELPER_SUGGESTION_H
