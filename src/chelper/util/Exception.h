//
// Created by Yancey on 2023/11/30.
//

#pragma once

#ifndef CHELPER_EXCEPTION_H
#define CHELPER_EXCEPTION_H

#include <string>
#include <vector>

namespace CHelper::Exception {

    class UnknownNodeId : public std::runtime_error {
    public:
        UnknownNodeId(const std::string &currentNodeId, const std::string &requiredNodeId);

        UnknownNodeId(const std::vector<std::string> &commandName, const std::string &requiredNodeId);
    };

}// namespace CHelper::Exception

#endif//CHELPER_EXCEPTION_H
