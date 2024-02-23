//
// Created by Yancey on 2023/11/30.
//

#ifndef CHELPER_PCH_H
#define CHELPER_PCH_H

#define CHelperLogger INFO
#define CHelperDebug true

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <exception>
#include <scoped_allocator>
#include <algorithm>
#include <fstream>
#include <windows.h>
#include <commctrl.h>
#include <ctime>
#include "nlohmann/json.hpp"
#include "format/Format.h"
#include "../src/chelper/util/ColorStringBuilder.h"
#include "../src/chelper/util/Exception.h"
#include "../src/chelper/util/JsonUtil.h"
#include "../src/chelper/util/Profile.h"
#include "../src/chelper/util/SimpleLogger.h"
#include "../src/chelper/util/StringUtil.h"
#include "../src/chelper/util/VectorView.h"

#endif //CHELPER_PCH_H
