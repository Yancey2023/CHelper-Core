//
// Created by Yancey on 2024/2/12.
//

#include "ErrorReason.h"

namespace CHelper {

    namespace ErrorReasonLevel {

        ErrorReasonLevel maxLevel = ID_ERROR;

    } // ErrorReasonLevel

    ErrorReason::ErrorReason(ErrorReasonLevel::ErrorReasonLevel level,
                             const VectorView <Token> &tokens,
                             std::string errorReason)
            : level(level),
              tokens(tokens),
              errorReason(std::move(errorReason)) {}

    //命令后面有多余部分
    std::shared_ptr<ErrorReason> ErrorReason::excess(const VectorView <Token> &tokens,
                                                     const std::string &errorReason) {
        return std::make_shared<ErrorReason>(ErrorReasonLevel::EXCESS, tokens, errorReason);
    }

    //缺少空格
    std::shared_ptr<ErrorReason> ErrorReason::requireWhiteSpace(const VectorView <Token> &tokens) {
        return std::make_shared<ErrorReason>(ErrorReasonLevel::REQUIRE_WHITE_SPACE, tokens, "指令不完整，缺少空格");
    }

    //指令不完整
    std::shared_ptr<ErrorReason> ErrorReason::incomplete(const VectorView <Token> &tokens,
                                                         const std::string &errorReason) {
        return std::make_shared<ErrorReason>(ErrorReasonLevel::INCOMPLETE, tokens, errorReason);
    }

    //类型不匹配
    std::shared_ptr<ErrorReason> ErrorReason::typeError(const VectorView <Token> &tokens,
                                                        const std::string &errorReason) {
        return std::make_shared<ErrorReason>(ErrorReasonLevel::TYPE_ERROR, tokens, errorReason);
    }

    //内容不匹配
    std::shared_ptr<ErrorReason> ErrorReason::contentError(const VectorView <Token> &tokens,
                                                           const std::string &errorReason) {
        return std::make_shared<ErrorReason>(ErrorReasonLevel::CONTENT_ERROR, tokens, errorReason);
    }

    //逻辑错误
    std::shared_ptr<ErrorReason> ErrorReason::logicError(const VectorView <Token> &tokens,
                                                         const std::string &errorReason) {
        return std::make_shared<ErrorReason>(ErrorReasonLevel::LOGIC_ERROR, tokens, errorReason);
    }

    //ID错误
    std::shared_ptr<ErrorReason> ErrorReason::idError(const VectorView <Token> &tokens,
                                                      const std::string &errorReason) {
        return std::make_shared<ErrorReason>(ErrorReasonLevel::ID_ERROR, tokens, errorReason);
    }

    bool ErrorReason::operator==(const ErrorReason &reason) const {
        return tokens.start == reason.tokens.start &&
               tokens.end == reason.tokens.end &&
               errorReason == reason.errorReason;
    }

} // CHelper

