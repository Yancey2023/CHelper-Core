//
// Created by Yancey on2024-5-21.
//

#include <gtest/gtest.h>

#include "../chelper/old2new/Old2New.h"
#include "param_deliver.h"

namespace CHelper::Test {

    TEST(Old2NewTest, Old2New) {
        std::filesystem::path projectDir(PROJECT_DIR);
        Old2New::BlockFixData blockFixData = Old2New::blockFixDataFromJson(JsonUtil::getJsonFromFile(projectDir / "resources" / "old2new" / "blockFixData.json"));
        std::vector<std::string> oldCommands = {
                R"(execute @e[x=~5] ~~~ detect ~~-1~ stone 0 setblock ~~1~ command_block 0)",
                R"(execute @e[type=zombie] ~ ~ ~ summon lightning_bolt)",
                R"(execute @e[type=zombie] ~ ~ ~ detect ~ ~-1 ~ minecraft:sand -1 summon lightning_bolt)",
                R"(execute @e[c=10] ~ ~ ~ execute @p ~ ~ ~ summon creeper ~ ~ ~)",
                R"(execute Yancey ~ ~ ~ summon ender_dragon)",
                R"(execute @e[x=~5] ~~~ detect ~~-1~ stone 1 setblock ~~1~ stone 2)",
                R"(setblock ~~~ stone)",
                R"(setblock ~~~ minecraft:command_block 1)",
                R"(setblock ~~~ stone 3)",
                R"(setblock ~~~ stone 3 replace)",
                R"(fill ~~~~~~ stone)",
                R"(fill ~~~~~~ stone 4)",
                R"(fill ~~~~~~ stone 4 hollow)",
                R"(fill ~~~~~~ stone 4 replace stone)",
                R"(fill ~~~~~~ stone 4 replace stone 5)",
                R"(testforblock ~~~ stone)",
                R"(testforblock ~~~ stone 3)",
                R"(testforblock ~~~ stone 3 replace)",
                R"(/execute @e[name="Yancey NB"] ~~2.5 ~ detect ~~-1~ stone 1 /setblock ~ ~-1 ~ command_block 0)",
                R"(execute @a[tag=!OP] ~~~ detect ~~0.05~0.3 air 0 execute @s ~~~ detect ~-0.3~-0.05~ air 0 execute @s ~~~ detect ~~-0.05~0.3 air 0 execute @s ~~~ detect ~0.3~-0.05~0.3 air 0 execute @s ~~~ detect ~-0.3~-0.05~-0.3 air 0 execute @s ~~~ detect ~0.3~-0.05~-0.3 air 0 execute @s ~~~ detect ~-0.3~-0.05~0.3 air 0 scoreboard players add @s fly 1)",
                R"(summon creeper ~ ~ ~ minecraft:become_charged "充能苦力怕")",
                R"(structure load aaa 0 0 0 0_degrees none true true 0.5 aaa)",
                R"(setblock ~~~ acacia_door["direction":1])",
        };
        for (const auto &item: oldCommands) {
            fmt::print("{}\n{}\n",
                       styled(item, fg(fmt::color::red)),
                       styled(Old2New::old2new(blockFixData, item), fg(fmt::color::lime_green)));
        }
    }
}// namespace CHelper::Test