#pragma once

#ifndef CHELPER_KMPMATCHER_H
#define CHELPER_KMPMATCHER_H

#include <iostream>
#include <string_view>
#include <vector>

namespace CHelper {

    class KMPMatcher {
    private:
        std::wstring_view pattern;
        std::vector<size_t> next;

    public:
        explicit KMPMatcher(const std::wstring_view &pattern);

        size_t match(const std::wstring &str);
    };


}// namespace CHelper

#endif// CHELPER_KMPMATCHER_H
