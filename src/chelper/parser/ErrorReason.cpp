//
// Created by Yancey on 2024/2/12.
//

#include "ErrorReason.h"

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
                             const TokensView &tokens,
                             std::string errorReason)
        : level(level),
          start(tokens.getStartIndex()),
          end(tokens.getEndIndex()),
          errorReason(std::move(errorReason)) {}

    bool ErrorReason::operator==(const ErrorReason &reason) const {
        return start == reason.start &&
               end == reason.end &&
               errorReason == reason.errorReason;
    }

}// namespace CHelper
