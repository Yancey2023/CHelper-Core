//
// Created by Yancey on 2024/2/21.
//

#include "Suggestion.h"

namespace CHelper {

    Suggestion::Suggestion(const VectorView <Token> &tokens,
                           const std::shared_ptr<NormalId> &content)
            : tokens(tokens),
              content(content) {}

} // CHelper