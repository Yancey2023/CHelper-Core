//
// Created by Yancey666 on 2023/11/7.
//

#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "BlockId.h"

CHelper::BlockStateValue::BlockStateValue(std::string value,
                                          const std::optional<std::string> &description)
        : valueString(std::move(value)),
          description(description),
          type(CHelper::BlockStateType::STRING) {}

CHelper::BlockStateValue::BlockStateValue(bool value,
                                          const std::optional<std::string> &description)
        : valueBoolean(value),
          description(description),
          type(CHelper::BlockStateType::BOOLEAN) {}

CHelper::BlockStateValue::BlockStateValue(int value,
                                          const std::optional<std::string> &description)
        : valueInteger(value),
          description(description),
          type(CHelper::BlockStateType::INTEGER) {}

CHelper::BlockState::BlockState(std::string key,
                                const std::optional<std::string> &description,
                                const std::vector<BlockStateValue> &values,
                                int defaultValue)
        : key(std::move(key)),
          description(description),
          values(values),
          defaultValue(defaultValue) {}

CHelper::BlockId::BlockId(const std::optional<std::__cxx11::basic_string<char>> &nameSpace,
                          const std::__cxx11::basic_string<char> &name,
                          const std::optional<std::__cxx11::basic_string<char>> &description,
                          const std::vector<BlockState> &blockStates,
                          const std::optional<int> &max,
                          const std::optional<std::vector<std::string>> &descriptions)
        : nameSpace(nameSpace),
          name(name),
          description(description),
          blockStates(blockStates),
          max(max),
          descriptions(descriptions) {
}
