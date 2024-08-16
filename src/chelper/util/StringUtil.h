//
// Created by Yancey on 2024/2/13.
//

#pragma once

#ifndef CHELPER_STRINGUTIL_H
#define CHELPER_STRINGUTIL_H

#include <string>
#include <vector>

namespace CHelper::StringUtil {

    std::wstring join(const std::wstring &joining, const std::vector<std::wstring> &strings);

}// namespace CHelper::StringUtil

#endif//CHELPER_STRINGUTIL_H