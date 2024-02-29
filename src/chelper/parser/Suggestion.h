//
// Created by Yancey on 2024/2/21.
//

#ifndef CHELPER_SUGGESTION_H
#define CHELPER_SUGGESTION_H

#include "../util/VectorView.h"
#include "../resources/id/NormalId.h"
#include "../lexer/Token.h"

namespace CHelper {

    class Suggestion {
    public:
        //TODO 为代码补全建议添加优先级
        //要被替换的位置
        size_t start, end;
        //内容
        std::shared_ptr<NormalId> content;

        Suggestion(size_t start, size_t end, const std::shared_ptr<NormalId> &content);

        Suggestion(const VectorView <Token> &tokens, const std::shared_ptr<NormalId> &content);

        std::string onClick(const std::string &before);
    };

} // CHelper

#endif //CHELPER_SUGGESTION_H
