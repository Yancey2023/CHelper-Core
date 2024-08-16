//
// Created by Yancey on 2024/2/21.
//

#include "Suggestion.h"
#include "../Core.h"
#include "Parser.h"

namespace CHelper {

    Suggestion::Suggestion(size_t start,
                           size_t end,
                           bool isAddWhitespace,
                           const std::shared_ptr<NormalId> &content)
        : start(start),
          end(end),
          isAddWhitespace(isAddWhitespace),
          content(content),
          mHashCode(31 * 31 * content->hashCode() + 31 * start + end) {}

    Suggestion::Suggestion(const TokensView &tokens,
                           bool isAddWhitespace,
                           const std::shared_ptr<NormalId> &content)
        : start(tokens.getStartIndex()),
          end(tokens.getEndIndex()),
          isAddWhitespace(isAddWhitespace),
          content(content),
          mHashCode(31 * 31 * content->hashCode() + 31 * start + end) {}

    std::pair<std::wstring, size_t> Suggestion::apply(Core *core, const std::wstring_view &before) const {
        if (content->name == L" " && (start == 0 || before[start - 1] == L' ')) {
            return {std::wstring(before), start};
        }
        std::pair<std::wstring, size_t> result = {
                std::wstring().append(before.substr(0, start)).append(content->name).append(before.substr(end)),
                start + content->name.length()};
        if (HEDLEY_UNLIKELY(end != before.length())) {
            return result;
        }
        core->onTextChanged(result.first, result.second);
        const ASTNode *astNode = core->getAstNode();
        if (HEDLEY_LIKELY(isAddWhitespace && astNode->isAllWhitespaceError())) {
            result.first.append(L" ");
            result.second++;
        }
        return result;
    }

}// namespace CHelper