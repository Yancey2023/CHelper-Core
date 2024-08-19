//
// Created by Yancey on2024-5-21.
//

#include "../chelper/Core.h"
#include <gtest/gtest.h>

void testCommand(CHelper::Core *core, const std::wstring &command) {
    core->onTextChanged(command, command.length());
    CHelper::ColoredString coloredString = core->getColors();
    std::wstring stringBuilder;
    for (int i = 0; i < coloredString.colors.size(); ++i) {
        uint32_t color = coloredString.colors[i];
        if (color == CHelper::NO_COLOR) {
            color = 0xFFFFFFFF;
        }
        stringBuilder.append(fmt::format(L"{}", fmt::styled(coloredString.str[i], fg(fmt::rgb(color)))));
    }
    std::wcout << stringBuilder << std::endl;
}

TEST(ColorStringTest, ColorString) {
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Core *core = CHelper::Core::createByDirectory(projectDir / "resources" / "beta" / "vanilla");
    auto commands = std::vector<std::wstring>{
            LR"(execute run clear )",
            LR"(give @s[hasitem=[{item=air,data=1},{item=minecraft:bed}],has_property={minecraft:is_rolled_up=true,m)",
            LR"(give @s command_block 112 12 {"minecraft:can_place_on":{"blocks":[")",
            LR"(tellraw @a {"rawtext":[{"text":"aaa","selector":"@a[type=\")",
            LR"(execute if block 12~23~)",
            LR"(give @s stone 12 21 {"minecraft:item_lock":{"mode":"l)",
            LR"(execute if block ~~~ anvil["aaa"=90.5] run g)",
            LR"(setblock ~1~2~3 stone[)",
            LR"(give @s stone 12 1)",
            LR"(tag @s add "\\\"\u1110\/\b\f\n\r\t\p\")",
            LR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":["minecraft:acacia_door"]}})",
            LR"(give @s s)",
            LR"(give @s stone 1 1 {)",
            LR"(give @s 石头)",
            LR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":[")",
            LR"(give)",
            LR"(give @)",
            LR"(give @a[x=^2,y=2,has_property={}])",
            LR"(give @a[x=^,has_property={""=!..12,="..}})",
            LR"(execute as @a run)",
            LR"(execute run)",
            LR"(execute if block ~~~ command_block run)",
            LR"(execute if block ~~~ bamboo)",
            LR"(give @s apple 12 1)",
            LR"(spreadplayers ~ ~ 0 1200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000)",
            LR"(camerashake add @a 10000000000000000000000000000000000000000000000000000 3402823466385288598117041834845169254401)",
            LR"(setblock ~~~ candle_cake[lit=)",
            LR"(give @s repeating_command_block)",
    };
    for (const auto &command: commands) {
        testCommand(core, command);
    }
    delete core;
}
