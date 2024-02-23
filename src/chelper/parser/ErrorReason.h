//
// Created by Yancey on 2024/2/12.
//

#ifndef CHELPER_ERRORREASON_H
#define CHELPER_ERRORREASON_H


#include "pch.h"
#include "../lexer/Token.h"
#include "../util/VectorView.h"

namespace CHelper {

    class ErrorReason {
    public:
        int level;
        VectorView<Token> tokens;
        std::string errorReason;

        ErrorReason(int level, VectorView<Token> tokens, std::string errorReason);

        static std::shared_ptr<ErrorReason> incomplete(VectorView <Token> tokens, const std::string &errorReason);

        static std::shared_ptr<ErrorReason> errorType(VectorView <Token> tokens, const std::string &errorReason);

        static std::shared_ptr<ErrorReason> errorContent(VectorView <Token> tokens, const std::string &errorReason);

        static std::shared_ptr<ErrorReason> other(VectorView <Token> tokens, const std::string &errorReason);

        static std::shared_ptr<ErrorReason> idError(VectorView <Token> tokens, const std::string &errorReason);

        static std::shared_ptr<ErrorReason> excess(VectorView <Token> tokens, const std::string &errorReason);

        bool operator==(const CHelper::ErrorReason &reason) const;

    };

} // CHelper

#endif //CHELPER_ERRORREASON_H
