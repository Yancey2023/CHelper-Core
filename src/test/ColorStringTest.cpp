//
// Created by Yancey on2024-5-21.
//

#include "../chelper/Core.h"
#include <gtest/gtest.h>

void testCommand(CHelper::Core *core, const std::u16string &command) {
    core->onTextChanged(command, command.length());
    CHelper::ColoredString coloredString = core->getColors();
    std::u16string stringBuilder;
    for (int i = 0; i < coloredString.colors.size(); ++i) {
        uint32_t color = coloredString.colors[i];
        if (color == CHelper::NO_COLOR) {
            color = 0xFFFFFFFF;
        }
        stringBuilder.append(fmt::format(u"{}", fmt::styled(coloredString.str[i], fg(fmt::rgb(color)))));
    }
    std::wcout << stringBuilder << std::endl;
}

TEST(ColorStringTest, ColorString) {
    std::filesystem::path projectDir(PROJECT_DIR);
    CHelper::Core *core = CHelper::Core::createByDirectory(projectDir / "resources" / "beta" / "vanilla");
    auto commands = std::vector<std::u16string>{
            uR"(execute run clear )",
            uR"(give @s[hasitem=[{item=air,data=1},{item=minecraft:bed}],has_property={minecraft:is_rolled_up=true,m)",
            uR"(give @s command_block 112 12 {"minecraft:can_place_on":{"blocks":[")",
            uR"(tellraw @a {"rawtext":[{"text":"aaa","selector":"@a[type=\")",
            uR"(execute if block 12~23~)",
            uR"(give @s stone 12 21 {"minecraft:item_lock":{"mode":"l)",
            uR"(execute if block ~~~ anvil["aaa"=90.5] run g)",
            uR"(setblock ~1~2~3 stone[)",
            uR"(give @s stone 12 1)",
            uR"(tag @s add "\\\"\u1110\/\b\f\n\r\t\p\")",
            uR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":["minecraft:acacia_door"]}})",
            uR"(give @s s)",
            uR"(give @s stone 1 1 {)",
            uR"(give @s 石头)",
            uR"(give @s command_block 12 12 {"minecraft:can_destroy":{"blocks":[")",
            uR"(give)",
            uR"(give @)",
            uR"(give @a[x=^2,y=2,has_property={}])",
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
    };
    for (const auto &command: commands) {
        testCommand(core, command);
    }
    delete core;
}
