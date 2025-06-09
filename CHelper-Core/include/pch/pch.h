//
// Created by Yancey on 2023/11/30.
//

#pragma once

#ifndef CHELPER_PCH_H
#define CHELPER_PCH_H

#include <ParamDeliver.h>

#if _CHELPER_DEBUG == true
#define CHelperDebug
#endif

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#if defined(CHelperAndroid) || defined(CHELPER_NO_FILESYSTEM)
#define FORMAT_ARG(arg) arg
#else
#define FORMAT_ARG(arg) fmt::styled(arg, fg(fmt::color::medium_purple))
#endif

#ifdef CHelperAndroid
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <jni.h>
#endif

// 数据结构
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
// 抛出的错误
#include <exception>
// 文件读写
#ifndef CHELPER_NO_FILESYSTEM
#include <filesystem>
#endif
// 用于字符串转整数或小数
#include <cinttypes>
// 字符串格式化
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/xchar.h>
// 日志库
#include <spdlog/spdlog.h>
// 开启编译器特性
#include <hedley.h>
// UTF编码处理库
#include <utf8.h>
// 序列化
#ifdef CHELPER_NO_FILESYSTEM
#define SERIALIZATION_NO_FILESYSTEM
#endif
#include <serialization/serialization.h>
// json工具
#include <chelper/util/JsonUtil.h>
// 简单的调用栈
#include <chelper/util/Profile.h>
// 字符串工具类
#include <chelper/util/StringUtil.h>
// KMP字符串匹配算法
#include <chelper/util/KMPMatcher.h>

#endif// CHELPER_PCH_H
