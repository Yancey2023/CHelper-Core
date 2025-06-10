//
// Created by Yancey on 2024/2/21.
//

#include <chelper/CHelperCore.h>
#include <chelper/parser/Parser.h>
#include <chelper/parser/Suggestion.h>

namespace CHelper {

    Suggestion::Suggestion(size_t start,
                           size_t end,
                           bool isAddSpace,
                           const std::shared_ptr<NormalId> &content)
        : start(start),
          end(end),
          isAddSpace(isAddSpace),
          content(content),
          mHashCode(31 * 31 * content->hashCode() + 31 * start + end) {}

    Suggestion::Suggestion(const TokensView &tokens,
                           bool isAddSpace,
                           const std::shared_ptr<NormalId> &content)
        : start(tokens.getStartIndex()),
          end(tokens.getEndIndex()),
          isAddSpace(isAddSpace),
          content(content),
          mHashCode(31 * 31 * content->hashCode() + 31 * start + end) {}

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

}// namespace CHelper