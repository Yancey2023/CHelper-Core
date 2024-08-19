//
// Created by Yancey on2024-5-21.
//

#include <gtest/gtest.h>

#include "../chelper/old2new/Old2New.h"

namespace CHelper::Test {

    TEST(Old2NewTest, Old2New) {
        std::filesystem::path projectDir(PROJECT_DIR);
        Old2New::BlockFixData blockFixData = Old2New::blockFixDataFromJson(JsonUtil::getJsonFromFile(projectDir / "resources" / "old2new" / "blockFixData.json"));
        std::vector<std::wstring> oldCommands = {
                LR"(execute @e[x=~5] ~~~ detect ~~-1~ stone 0 setblock ~~1~ command_block 0)",
                LR"(execute @e[type=zombie] ~ ~ ~ summon lightning_bolt)",
                LR"(execute @e[type=zombie] ~ ~ ~ detect ~ ~-1 ~ minecraft:sand -1 summon lightning_bolt)",
                LR"(execute @e[c=10] ~ ~ ~ execute @p ~ ~ ~ summon creeper ~ ~ ~)",
                LR"(execute Yancey ~ ~ ~ summon ender_dragon)",
                LR"(execute @e[x=~5] ~~~ detect ~~-1~ stone 1 setblock ~~1~ stone 2)",
                LR"(setblock ~~~ stone)",
                LR"(setblock ~~~ minecraft:command_block 1)",
                LR"(setblock ~~~ stone 3)",
                LR"(setblock ~~~ stone 3 replace)",
                LR"(fill ~~~~~~ stone)",
                LR"(fill ~~~~~~ stone 4)",
                LR"(fill ~~~~~~ stone 4 hollow)",
                LR"(fill ~~~~~~ stone 4 replace stone)",
                LR"(fill ~~~~~~ stone 4 replace stone 5)",
                LR"(testforblock ~~~ stone)",
                LR"(testforblock ~~~ stone 3)",
                LR"(testforblock ~~~ stone 3 replace)",
                LR"(/execute @e[name="Yancey NB"] ~~2.5 ~ detect ~~-1~ stone 1 /setblock ~ ~-1 ~ command_block 0)",
                LR"(execute @a[tag=!OP] ~~~ detect ~~0.05~0.3 air 0 execute @s ~~~ detect ~-0.3~-0.05~ air 0 execute @s ~~~ detect ~~-0.05~0.3 air 0 execute @s ~~~ detect ~0.3~-0.05~0.3 air 0 execute @s ~~~ detect ~-0.3~-0.05~-0.3 air 0 execute @s ~~~ detect ~0.3~-0.05~-0.3 air 0 execute @s ~~~ detect ~-0.3~-0.05~0.3 air 0 scoreboard players add @s fly 1)",
                LR"(summon creeper ~ ~ ~ minecraft:become_charged "充能苦力怕")",
                LR"(structure load aaa 0 0 0 0_degrees none true true 0.5 aaa)",
                LR"(setblock ~~~ acacia_door["direction":1])",
        };
        for (const auto &item: oldCommands) {
            fmt::print("{}\n{}\n",
                       styled(wstring2string(item), fg(fmt::color::red)),
                       styled(wstring2string(Old2New::old2new(blockFixData, item)), fg(fmt::color::lime_green)));
        }
    }
}// namespace CHelper::Test