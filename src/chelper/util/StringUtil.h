//
// Created by Yancey666 on 2024/2/13.
//

#ifndef CHELPER_STRINGUTIL_H
#define CHELPER_STRINGUTIL_H

#include <string>
#include <vector>

namespace CHelper::StringUtil {

    std::string join(const std::string &joining, const std::vector<std::string> &strings);

    bool isStartOf(std::string_view a, std::string_view b);

} //CHelper::StringUtil

#endif //CHELPER_STRINGUTIL_H