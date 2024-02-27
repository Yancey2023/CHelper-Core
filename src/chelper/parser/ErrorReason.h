//
// Created by Yancey on 2024/2/12.
//

#ifndef CHELPER_ERRORREASON_H
#define CHELPER_ERRORREASON_H


#include "pch.h"
#include "../lexer/Token.h"
#include "../util/VectorView.h"

namespace CHelper {

    namespace ErrorReasonLevel {

        enum ErrorReasonLevel : std::uint8_t{
            //命令后面有多余部分
            EXCESS = 0,
            //缺少空格
            REQUIRE_WHITE_SPACE = 1,
            //指令不完整
            INCOMPLETE = 2,
            //类型不匹配
            TYPE_ERROR = 3,
            //内容不匹配
            CONTENT_ERROR = 4,
            //逻辑错误
            LOGIC_ERROR = 5,
            //ID错误
            ID_ERROR = 6
        };

        //最大的错误等级，在ErrorReason.cpp中有定义
        extern ErrorReasonLevel maxLevel;

    }

    class ErrorReason {
    public:
        ErrorReasonLevel::ErrorReasonLevel level;
        VectorView <Token> tokens;
        std::string errorReason;

        ErrorReason(ErrorReasonLevel::ErrorReasonLevel level,
                    const VectorView <Token> &, std::string errorReason);

        static std::shared_ptr<ErrorReason> excess(const VectorView <Token> &tokens,
                                                   const std::string &errorReason);

        static std::shared_ptr<ErrorReason> requireWhiteSpace(const VectorView <Token> &tokens);

        static std::shared_ptr<ErrorReason> incomplete(const VectorView <Token> &tokens,
                                                       const std::string &errorReason);

        static std::shared_ptr<ErrorReason> typeError(const VectorView <Token> &tokens,
                                                      const std::string &errorReason);

        static std::shared_ptr<ErrorReason> contentError(const VectorView <Token> &tokens,
                                                         const std::string &errorReason);

        static std::shared_ptr<ErrorReason> logicError(const VectorView <Token> &tokens,
                                                       const std::string &errorReason);

        static std::shared_ptr<ErrorReason> idError(const VectorView <Token> &tokens,
                                                    const std::string &errorReason);

        bool operator==(const CHelper::ErrorReason &reason) const;

    };

} // CHelper

#endif //CHELPER_ERRORREASON_H
