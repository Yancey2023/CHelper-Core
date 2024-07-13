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

    std::string Suggestion::apply(Core *core, const std::string &before) const {
        if (content->name == " " && (start == 0 || before[start - 1] == ' ')) {
            return before;
        }
        std::string result = before.substr(0, start)
                                     .append(content->name)
                                     .append(before.substr(end));
        if (HEDLEY_UNLIKELY(end != before.length())) {
            return std::move(result);
        }
        core->onTextChanged(result, result.size());
        const ASTNode *astNode = core->getAstNode();
        if (HEDLEY_LIKELY(isAddWhitespace && astNode->isAllWhitespaceError())) {
            result.push_back(' ');
        }
        return std::move(result);
    }

}// namespace CHelper