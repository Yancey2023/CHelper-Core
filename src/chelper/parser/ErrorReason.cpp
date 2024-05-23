//
// Created by Yancey on 2024/2/12.
//

#include "ErrorReason.h"
#include "../util/TokenUtil.h"

namespace CHelper {

    namespace ErrorReasonLevel {

        ErrorReasonLevel maxLevel = ID_ERROR;

    }// namespace ErrorReasonLevel

    ErrorReason::ErrorReason(ErrorReasonLevel::ErrorReasonLevel level,
                             size_t start,
                             size_t end,
                             std::string errorReason)
        : level(level),
          start(start),
          end(end),
          errorReason(std::move(errorReason)) {}

    ErrorReason::ErrorReason(ErrorReasonLevel::ErrorReasonLevel level,
                             const VectorView<Token> &tokens,
                             std::string errorReason)
        : level(level),
          start(TokenUtil::getStartIndex(tokens)),
          end(TokenUtil::getEndIndex(tokens)),
          errorReason(std::move(errorReason)) {}

    bool ErrorReason::operator==(const ErrorReason &reason) const {
        return start == reason.start &&
               end == reason.end &&
               errorReason == reason.errorReason;
    }

}// namespace CHelper
