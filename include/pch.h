//
// Created by Yancey on 2023/11/30.
//

#pragma once

#ifndef CHELPER_PCH_H
#define CHELPER_PCH_H

#define CHelperLogger INFO

// 可以在运行时增加一些检测，快速定位错误
#define CHelperDebug true

// 增加一些用于调试方法
#define CHelperTest false

// 去除一些没有必要的代码，减少体积
#define CHelperWeb false

#if CHelperWeb == true
#define CHelperSupportJson false
#else
#define CHelperSupportJson true
#endif

#if CHelperAndroid == true

#include "android/asset_manager.h"
#include "android/asset_manager_jni.h"
#include "android/jni.h"
#include "android/log.h"

#endif

// 数据结构
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <functional>
#include <cmath>
// 抛出的错误
#include <exception>
// 文件读写
#if CHelperSupportJson == true
#include <fstream>
#endif
// 用于字符串转整数或小数
#include <cinttypes>
// json库
#if CHelperSupportJson == true
#include "nlohmann/json.hpp"
#else
#include "nlohmann/detail/macro_scope.hpp"
#endif
// 字符串格式化
#include "format/Format.h"
// 开始编译器特性
#include "hedley.h"
// 二进制读写
#include "../src/chelper/util/BinaryUtil.h"
// 一些处理json和二进制序列化的宏
#include "../src/chelper/util/Codec.h"
// 有颜色的字符串，用于控制台显示
#include "../src/chelper/util/ColorStringBuilder.h"
// json读写
#include "../src/chelper/util/JsonUtil.h"
// 简单的调用栈
#include "../src/chelper/util/Profile.h"
// 简单的日志系统
#include "../src/chelper/util/SimpleLogger.h"
// 字符串工具类
#include "../src/chelper/util/StringUtil.h"
// KMP字符串匹配算法
#include "../src/chelper/util/KMPMatcher.h"

#endif // CHELPER_PCH_H
