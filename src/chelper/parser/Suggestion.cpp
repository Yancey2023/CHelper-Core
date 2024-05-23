//
// Created by Yancey on 2024/2/21.
//

#include "Suggestion.h"
#include "../Core.h"
#include "../util/TokenUtil.h"
#include "Parser.h"

namespace CHelper {

    Suggestion::Suggestion(size_t start,
                           size_t end,
                           const std::shared_ptr<NormalId> &content)
        : start(start),
          end(end),
          content(content),
          mHashCode(31 * 31 * content->hashCode() + 31 * start + end) {}

    Suggestion::Suggestion(const VectorView<Token> &tokens,
                           const std::shared_ptr<NormalId> &content)
        : start(TokenUtil::getStartIndex(tokens)),
          end(TokenUtil::getEndIndex(tokens)),
          content(content),
          mHashCode(31 * 31 * content->hashCode() + 31 * start + end) {}

    std::string Suggestion::onClick(Core *core, const std::string &before) const {
        std::string result = before.substr(0, start)
                                     .append(content->name)
                                     .append(before.substr(end));
        if (HEDLEY_UNLIKELY(end != before.length())) {
            return std::move(result);
        }
        core->onTextChanged(result, result.size());
        const ASTNode *astNode = core->getAstNode();
        if (HEDLEY_LIKELY(astNode->canAddWhitespace && astNode->isAllWhitespaceError())) {
            result.push_back(' ');
        }
        return std::move(result);
    }

}// namespace CHelper