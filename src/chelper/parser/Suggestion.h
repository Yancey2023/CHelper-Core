//
// Created by Yancey on 2024/2/21.
//

#pragma once

#ifndef CHELPER_SUGGESTION_H
#define CHELPER_SUGGESTION_H

#include "../util/VectorView.h"
#include "../resources/id/NormalId.h"
#include "../lexer/Token.h"

namespace CHelper {

    class Core;

    class Suggestion {
    public:
        //TODO 为代码补全建议添加优先级
        //要被替换的位置
        size_t start, end;
        //内容
        std::shared_ptr<NormalId> content;
    private:
        const size_t mHashCode;

    public:
        Suggestion(size_t start, size_t end, const std::shared_ptr<NormalId> &content);

        Suggestion(const VectorView <Token> &tokens, const std::shared_ptr<NormalId> &content);

        [[nodiscard]] std::string onClick(Core* core, const std::string &before) const;

        [[nodiscard]] inline size_t hashCode() const {
            return mHashCode;
        }

        [[nodiscard]] inline bool equal(const Suggestion &suggestion) const {
            return mHashCode == suggestion.mHashCode;
        }

    };

} // CHelper

#endif //CHELPER_SUGGESTION_H
