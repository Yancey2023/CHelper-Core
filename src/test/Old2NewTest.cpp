//
// Created by Yancey on 24-5-21.
//

#include <gtest/gtest.h>

#include "../chelper/old2new/Old2New.h"

namespace CHelper::Test {

    TEST(Old2NewTest, Old2New) {
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
        };
        for (const auto &item: oldCommands){
            std::cout << ColorStringBuilder().red(item + "\n").green(Old2New::old2new(item)).build() << std::endl;
        }
    }
}// namespace CHelper::Test