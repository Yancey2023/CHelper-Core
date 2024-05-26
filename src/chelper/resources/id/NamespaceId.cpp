//
// Created by Yancey on 2023/11/11.
//

#include "NamespaceId.h"

namespace CHelper {

    std::shared_ptr<NormalId> &NamespaceId::getIdWithNamespace() {
        if (HEDLEY_UNLIKELY(idWithNamespace == nullptr)) {
            idWithNamespace = NormalId::make(
                    std::string(idNamespace.value_or("minecraft"))
                            .append(":")
                            .append(name),
                    description);
        }
        return idWithNamespace;
    }

    CODEC_WITH_PARENT(NamespaceId, NormalId, idNamespace)

}// namespace CHelper