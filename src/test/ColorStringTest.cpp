//
// Created by Yancey on2024-5-21.
//

#include "../chelper/Core.h"
#include "param_deliver.h"
#include <gtest/gtest.h>

void testCommand(CHelper::Core *core, const std::string &command) {
    core->onTextChanged(command, command.length());
    CHelper::ColoredString coloredString = core->getColors();
    std::string stringBuilder;
    for (int i = 0; i < coloredString.colors.size(); ++i) {
        uint32_t color = coloredString.colors[i];
        if (color == CHelper::NO_COLOR) {
            color = 0xFFFFFFFF;
        }
        stringBuilder.append(fmt::format("{}", fmt::styled(coloredString.str[i], fg(fmt::rgb(color)))));
    }
    std::cout << stringBuilder << std::endl;
}

TEST(ColorStringTest, ColorString) {
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Core *core = CHelper::Core::createByDirectory(projectDir / "resources" / "beta" / "vanilla");
    auto commands = std::vector<std::string>{
            R"(execute run clear )",
            R"(give @s[hasitem=[{item=air,data=1},{item=minecraft:bed}],has_property={minecraft:is_rolled_up=true,m)",
            R"(give @s command_block 112 12 {"minecraft:can_place_on":{"blocks":[")",
            R"(tellraw @a {"rawtext":[{"text":"aaa","selector":"@a[type=\")",
            R"(execute if block 12~23~)",
            R"(give @s stone 12 21 {"minecraft:item_lock":{"mode":"l)",
            R"(execute if block ~~~ anvil["aaa"=90.5] run g)",
            R"(setblock ~1~2~3 stone[)",
            R"(give @s stone 12 1)",
            R"(tag @s add "\\\"\u1110\/\b\f\n\r\t\p\")",
            R"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":["minecraft:acacia_door"]}})",
            R"(give @s s)",
            R"(give @s stone 1 1 {)",
            R"(give @s 石头)",
            R"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":[")",
            R"(give)",
            R"(give @)",
            R"(give @a[x=^2,y=2,has_property={}])",
            R"(give @a[x=^,has_property={""=!..12,="..}})",
            R"(execute as @a run)",
            R"(execute run)",
            R"(execute if block ~~~ command_block run)",
            R"(execute if block ~~~ bamboo)",
            R"(give @s apple 12 1)",
            R"(spreadplayers ~ ~ 0 1200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)",
            R"(camerashake add @a 10000000000000000000000000000000000000000000000000000 3402823466385288598117041834845169254401)",
            R"(setblock ~~~ candle_cake[lit=)",
            R"(give @s repeating_command_block)",
    };
    for (const auto &command: commands) {
        testCommand(core, command);
    }
    delete core;
}
