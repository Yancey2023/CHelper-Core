//
// Created by Yancey on 2024/2/21.
//

#include "Suggestion.h"
#include "../util/TokenUtil.h"

namespace CHelper {

    Suggestion::Suggestion(size_t start,
                           size_t end,
                           const std::shared_ptr<NormalId> &content)
            : start(start),
              end(end),
              content(content),
              mHashCode(HashUtil::combineHash(content->hashCode(), HashUtil::combineHash(start, end))) {}

    Suggestion::Suggestion(const VectorView <Token> &tokens,
                           const std::shared_ptr<NormalId> &content)
            : start(TokenUtil::getStartIndex(tokens)),
              end(TokenUtil::getEndIndex(tokens)),
              content(content),
              mHashCode(HashUtil::combineHash(content->hashCode(), HashUtil::combineHash(start, end))) {}

    std::string Suggestion::onClick(const std::string &before) const {
        return before.substr(0, start)
                .append(content->name)
                .append(before.substr(end));
    }

} // CHelper