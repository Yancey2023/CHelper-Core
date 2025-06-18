//
// Created by Yancey on 2024/2/21.
//

#include <chelper/CHelperCore.h>
#include <chelper/parser/Suggestion.h>

namespace CHelper {

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
        : start(tokens.getStartIndex()),
          end(tokens.getEndIndex()),
          isAddSpace(isAddSpace),
          content(content) {}

    std::pair<std::u16string, size_t> Suggestion::apply(CHelperCore *core, const std::u16string_view &before) const {
        if (content->name == u" " && (start == 0 || before[start - 1] == u' ')) {
            return {std::u16string(before), start};
        }
        std::pair<std::u16string, size_t> result = {
                std::u16string().append(before.substr(0, start)).append(content->name).append(before.substr(end)),
                start + content->name.length()};
        if (HEDLEY_UNLIKELY(end != before.length())) {
            return result;
        }
        core->onTextChanged(result.first, result.second);
        const ASTNode *astNode = core->getAstNode();
        if (HEDLEY_LIKELY(isAddSpace && astNode->isAllSpaceError())) {
            result.first.append(u" ");
            result.second++;
        }
        return result;
    }

    [[nodiscard]] XXH64_hash_t Suggestion::hashCode() const {
        XXH3_state_t hashState;
        XXH3_copyState(&hashState, content->getHashState());
        XXH3_64bits_update(&hashState, &start, sizeof(start));
        XXH3_64bits_update(&hashState, &end, sizeof(end));
        return XXH3_64bits_digest(&hashState);
    }

}// namespace CHelper