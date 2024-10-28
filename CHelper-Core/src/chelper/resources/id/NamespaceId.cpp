//
// Created by Yancey on 2023/11/11.
//

#include <chelper/resources/id/NamespaceId.h>

namespace CHelper {

    std::shared_ptr<NormalId> &NamespaceId::getIdWithNamespace() {
        if (HEDLEY_UNLIKELY(idWithNamespace == nullptr)) {
            idWithNamespace = NormalId::make(
                    idNamespace.value_or(u"minecraft")
                            .append(u":")
                            .append(name),
                    description);
        }
        return idWithNamespace;
    }

}// namespace CHelper