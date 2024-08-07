//
// Created by Yancey on 2023/11/30.
//

#pragma once

#ifndef CHELPER_PCH_H
#define CHELPER_PCH_H

#define CHelperLogger INFO

#ifndef NDEBUG
// 可以在运行时增加一些检测，快速定位错误
#define CHelperDebug true
// 增加一些用于调试方法
#define CHelperTest false
#endif

// 是否支持导入或导出JSON格式的资源包
#if CHelperWeb == true
#define CHelperOnlyReadBinary true
#else
#define CHelperOnlyReadBinary false
#endif
#define CHelperOnlyReadBinary true

// 由于测试过程需要用到JSON相关的东西，所以CHelperTest和CHelperOnlyReadBinary不能同时为true
#if CHelperTest == true && CHelperOnlyReadBinary == true
#error CHelperTest and CHelperOnlyReadBinary cannot both be true
#endif

#if CHelperAndroid == true

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <jni.h>

#endif

// 数据结构
#include <algorithm>
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
#if CHelperOnlyReadBinary != true
#include <fstream>
#endif
// 用于字符串转整数或小数
#include <cinttypes>
// json库
#if CHelperOnlyReadBinary == true
#include <nlohmann/detail/macro_scope.hpp>
#else
#include <nlohmann/json.hpp>
#endif
// 字符串格式化
#include <fmt/color.h>
#include <fmt/format.h>
// 开启编译器特性
#include <hedley.h>
// 二进制读写
#include "../../src/chelper/util/BinaryUtil.h"
// 一些处理json和二进制序列化的宏
#include "../../src/chelper/util/Codec.h"
// json读写
#include "../../src/chelper/util/JsonUtil.h"
// 简单的调用栈
#include "../../src/chelper/util/Profile.h"
// 简单的日志系统
#include "../../src/chelper/util/SimpleLogger.h"
// 字符串工具类
#include "../../src/chelper/util/StringUtil.h"
// KMP字符串匹配算法
#include "../../src/chelper/util/KMPMatcher.h"

#endif // CHELPER_PCH_H
