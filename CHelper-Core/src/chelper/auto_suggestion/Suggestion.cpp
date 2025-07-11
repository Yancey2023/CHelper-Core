//
// Created by Yancey on 2024/2/21.
//

#include <chelper/CHelperCore.h>
#include <chelper/auto_suggestion/Suggestion.h>

namespace CHelper::AutoSuggestion {

    Suggestion::Suggestion(size_t start,
                           size_t end,
                           bool isAddSpace,
                           const std::shared_ptr<NormalId> &content)
        : start(start),
          end(end),
          isAddSpace(isAddSpace),
          content(content) {}

    Suggestion::Suggestion(const TokensView &tokens,
                           bool isAddSpace,
                           const std::shared_ptr<NormalId> &content)
        : start(tokens.startIndex),
          end(tokens.endIndex),
          isAddSpace(isAddSpace),
          content(content) {}

    [[nodiscard]] XXH64_hash_t Suggestion::hashCode() const {
        XXH3_state_t hashState;
        XXH3_copyState(&hashState, content->getHashState());
        XXH3_64bits_update(&hashState, &start, sizeof(start));
        XXH3_64bits_update(&hashState, &end, sizeof(end));
        return XXH3_64bits_digest(&hashState);
    }

}// namespace CHelper