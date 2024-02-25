//
// Created by Yancey on 2024/2/21.
//

#include "Suggestion.h"

namespace CHelper {

    Suggestion::Suggestion(const VectorView <Token> &tokens,
                           const std::shared_ptr<NormalId> &content)
            : tokens(tokens),
              content(content) {}

    std::string Suggestion::onClick() {
        std::string result;
        for (size_t i = 0; i < tokens.start; ++i) {
            result.append(tokens.vector->at(i).content);
        }
        result.append(content->name);
        for (size_t i = tokens.end; i < tokens.vector->size(); ++i) {
            result.append(tokens.vector->at(i).content);
        }
        return result;
    }

} // CHelper