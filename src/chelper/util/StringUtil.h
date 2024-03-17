//
// Created by Yancey on 2024/2/13.
//

#pragma once

#ifndef CHELPER_STRINGUTIL_H
#define CHELPER_STRINGUTIL_H

#include <string>
#include <vector>

namespace CHelper::StringUtil {

    std::string join(const std::string &joining, const std::vector<std::string> &strings);

} //CHelper::StringUtil

#endif //CHELPER_STRINGUTIL_H