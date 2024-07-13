//
// Created by Yancey on 2023/11/30.
//

#include "Exception.h"
#include "ColorStringBuilder.h"
#include "Profile.h"
#include "SimpleLogger.h"

namespace CHelper::Exception {

    UnknownNodeId::UnknownNodeId(const std::string &currentNodeId, const std::string &requiredNodeId)
        : std::runtime_error(ColorStringBuilder()
                                     .red("unknown node id")
                                     .normal(" -> ")
                                     .purple(requiredNodeId)
                                     .red(" (in node \"")
                                     .purple(currentNodeId)
                                     .red("\")")
                                     .build()) {}

    UnknownNodeId::UnknownNodeId(const std::vector<std::string> &commandName, const std::string &requiredNodeId)
        : std::runtime_error(ColorStringBuilder()
                                     .red("unknown node id")
                                     .normal(" -> ")
                                     .purple(requiredNodeId)
                                     .red(" (in command ")
                                     .purple(StringUtil::join(",", commandName))
                                     .red(")")
                                     .build()) {}

}// namespace CHelper::Exception