//
// Created by Yancey on 2024/2/12.
//

#pragma once

#ifndef CHELPER_ERRORREASON_H
#define CHELPER_ERRORREASON_H


#include "../lexer/Token.h"
#include "TokensView.h"
#include "pch.h"

namespace CHelper {

    namespace ErrorReasonLevel {

        enum ErrorReasonLevel : uint8_t {
            //命令后面有多余部分
            EXCESS = 0,
            //缺少空格
            REQUIRE_WHITE_SPACE = 1,
            //命令不完整
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

    }// namespace ErrorReasonLevel

    class ErrorReason {
    public:
        ErrorReasonLevel::ErrorReasonLevel level;
        std::wstring errorReason;
        size_t start, end;

        ErrorReason(ErrorReasonLevel::ErrorReasonLevel level,
                    size_t start,
                    size_t end,
                    std::wstring errorReason);

        ErrorReason(ErrorReasonLevel::ErrorReasonLevel level,
                    const TokensView &,
                    std::wstring errorReason);

        //命令后面有多余部分
        [[maybe_unused]] static std::shared_ptr<ErrorReason> excess(size_t start,
                                                                    size_t end,
                                                                    const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::EXCESS, start, end, errorReason);
        }

        [[maybe_unused]] static std::shared_ptr<ErrorReason> excess(const TokensView &tokens,
                                                                    const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::EXCESS, tokens, errorReason);
        }

        //缺少空格
        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        requireWhiteSpace(const TokensView &tokens) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::REQUIRE_WHITE_SPACE, tokens, L"命令不完整，缺少空格");
        }

        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        requireWhiteSpace(size_t start, size_t end) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::REQUIRE_WHITE_SPACE, start, end, L"命令不完整，缺少空格");
        }

        //命令不完整
        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        incomplete(size_t start, size_t end, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::INCOMPLETE, start, end, errorReason);
        }

        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        incomplete(const TokensView &tokens, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::INCOMPLETE, tokens, errorReason);
        }

        //类型不匹配
        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        typeError(size_t start, size_t end, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::TYPE_ERROR, start, end, errorReason);
        }

        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        typeError(const TokensView &tokens, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::TYPE_ERROR, tokens, errorReason);
        }

        //内容不匹配
        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        contentError(size_t start, size_t end, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::CONTENT_ERROR, start, end, errorReason);
        }

        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        contentError(const TokensView &tokens, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::CONTENT_ERROR, tokens, errorReason);
        }

        //逻辑错误
        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        logicError(size_t start, size_t end, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::LOGIC_ERROR, start, end, errorReason);
        }

        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        logicError(const TokensView &tokens, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::LOGIC_ERROR, tokens, errorReason);
        }

        //ID错误
        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        idError(size_t start, size_t end, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::ID_ERROR, start, end, errorReason);
        }

        [[maybe_unused]] static std::shared_ptr<ErrorReason>
        idError(const TokensView &tokens, const std::wstring &errorReason) {
            return std::make_shared<ErrorReason>(ErrorReasonLevel::ID_ERROR, tokens, errorReason);
        }

        bool operator==(const CHelper::ErrorReason &reason) const;
    };

}// namespace CHelper

#endif//CHELPER_ERRORREASON_H
