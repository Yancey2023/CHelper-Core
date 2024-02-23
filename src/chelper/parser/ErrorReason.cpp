//
// Created by Yancey on 2024/2/12.
//

#include "ErrorReason.h"

namespace CHelper {

    ErrorReason::ErrorReason(int level,
                             VectorView <Token> tokens,
                             std::string errorReason)
            : level(level),
              tokens(tokens),
              errorReason(std::move(errorReason)) {}

    //指令不完整
    std::shared_ptr<ErrorReason> ErrorReason::incomplete(VectorView <Token> tokens, const std::string &errorReason) {
        return std::make_shared<ErrorReason>(0, tokens, errorReason);
    }

    //类型不匹配
    std::shared_ptr<ErrorReason> ErrorReason::errorType(VectorView <Token> tokens, const std::string &errorReason) {
        return std::make_shared<ErrorReason>(1, tokens, errorReason);
    }

    //内容不匹配
    std::shared_ptr<ErrorReason> ErrorReason::errorContent(VectorView <Token> tokens, const std::string &errorReason) {
        return std::make_shared<ErrorReason>(3, tokens, errorReason);
    }

    //其他原因
    std::shared_ptr<ErrorReason> ErrorReason::other(VectorView <Token> tokens, const std::string &errorReason) {
        return std::make_shared<ErrorReason>(4, tokens, errorReason);
    }

    //ID原因
    std::shared_ptr<ErrorReason> ErrorReason::idError(VectorView <Token> tokens, const std::string &errorReason) {
        return std::make_shared<ErrorReason>(5, tokens, errorReason);
    }

    //命令后面有多余部分
    std::shared_ptr<ErrorReason> ErrorReason::excess(VectorView <Token> tokens, const std::string &errorReason) {
        return std::make_shared<ErrorReason>(6, tokens, errorReason);
    }

    bool ErrorReason::operator==(const ErrorReason &reason) const {
        return tokens.start == reason.tokens.start &&
               tokens.end == reason.tokens.end &&
               errorReason == reason.errorReason;
    }

} // CHelper

