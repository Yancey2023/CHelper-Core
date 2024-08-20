//
// Created by Yancey on2024-5-21.
//

#include <gtest/gtest.h>

#include <chelper/CHelperCore.h>
#include <chelper/parser/Parser.h>

namespace CHelper::Test {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-result"

/**
 * 测试程序是否可以正常运行
 */
[[maybe_unused]] void test(const std::filesystem::path &cpackPath,
                           const std::vector<std::u16string> &commands) {
  std::shared_ptr<CHelperCore> core;
  try {
    std::unique_ptr<CPack> cPack = CPack::createByDirectory(cpackPath);
    ASTNode astNode = Parser::parse(u"", cPack.get());
    core = std::make_shared<CHelperCore>(std::move(cPack), std::move(astNode));
  } catch (const std::exception &e) {
    Profile::printAndClear(e);
    FAIL();
  }
  bool flag = false;
  for (const auto &command : commands) {
    try {
      core->onTextChanged(command, command.length());
      core->getDescription();
      core->getSuggestions();
      core->getSuggestions();
      core->getStructure();
    } catch (const std::exception &e) {
      CHELPER_INFO("parse command: {}", command);
      CHelper::Profile::printAndClear(e);
      flag = true;
    }
  }
  if (HEDLEY_UNLIKELY(flag)) {
    FAIL();
  }
}

#pragma clang diagnostic pop

void writeBson(const std::filesystem::path &input,
               const std::filesystem::path &output) {
  try {
    auto core = CHelperCore::createByDirectory(input);
    fmt::print("\n");
    if (HEDLEY_UNLIKELY(core == nullptr)) {
      return;
    }
    std::chrono::high_resolution_clock::time_point start, end;
    start = std::chrono::high_resolution_clock::now();
    core->getCPack()->writeBsonToFile(output);
    end = std::chrono::high_resolution_clock::now();
    CHELPER_INFO("write successfully({})",
                 std::to_string(
                     std::chrono::duration_cast<
                         std::chrono::duration<float, std::milli>>(end - start)
                         .count()) +
                     "ms");
    fmt::print("\n");
    [[maybe_unused]] auto core2 = CHelperCore::createByBson(output);
    fmt::print("\n");
  } catch (const std::exception &e) {
    Profile::printAndClear(e);
    throw e;
  }
}

} // namespace CHelper::Test

TEST(MainTest, ParseCommand) {
  std::filesystem::path resourceDir(RESOURCE_DIR);
  CHelper::Test::test(
      resourceDir / "resources" / "beta" / "vanilla",
      std::vector<std::u16string>{
          uR"(execute run clear )",
          uR"(give @s[hasitem=[{item=air,data=1},{item=minecraft:bed}],has_property={minecraft:is_rolled_up=true,m)",
          uR"(give @s command_block 112 12 {"minecraft:can_place_on":{"blocks":[")",
          uR"(tellraw @a {"rawtext":[{"text":"aaa","selector":"@a[type=\")",
          uR"(execute if block 12~23~)",
          uR"(give @s stone 12 21 {"minecraft:item_lock":{"mode":"l)",
          uR"(execute if block ~~~ anvil["aaa"=90.5] run g)",
          uR"(setblock ~~~ stone[)",
          uR"(give @s stone 12 1)",
          uR"(tag @s add "\\\"\u1110\/\b\f\n\r\t\p\")",
          uR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":["minecraft:acacia_door"]}})",
          uR"(give @s s)",
          uR"(give @s stone 1 1 {)",
          uR"(give @s 石头)",
          uR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":[")",
          uR"(give)",
          uR"(give @)",
          uR"(give @a[x=^,has_property={""=!..12,="..}})",
          uR"(execute as @a run)",
          uR"(execute run)",
          uR"(execute if block ~~~ command_block run)",
          uR"(execute if block ~~~ bamboo)",
          uR"(give @s apple 12 1)",
          uR"(spreadplayers ~ ~ 0 1200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)",
          uR"(camerashake add @a 10000000000000000000000000000000000000000000000000000 3402823466385288598117041834845169254401)",
          uR"(setblock ~~~ candle_cake[lit=)",
          uR"(give @s repeating_command_block)",
      });
}