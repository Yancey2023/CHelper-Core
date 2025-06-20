//
// Created by Yancey on 2024-5-21.
//

#include <chelper/CHelperCore.h>
#include <chelper/parser/Parser.h>
#include <gtest/gtest.h>

namespace CHelper::Test {

    const char *getTokenTypeStr(const CHelper::TokenType::TokenType &tokenType) {
        switch (tokenType) {
            case CHelper::TokenType::NUMBER:
                return "NUMBER";
            case CHelper::TokenType::STRING:
                return "STRING";
            case CHelper::TokenType::SYMBOL:
                return "SYMBOL";
            case CHelper::TokenType::SPACE:
                return "WHITESPACE";
            default:
                return "UNKNOWN";
        }
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void testLex(const std::vector<std::u16string> &commands) {
        try {
            for (const auto &command: commands) {
                fmt::println("lex command: {}", FORMAT_ARG(utf8::utf16to8(command)));
                for (const auto &item: Lexer::lex(command).allTokens) {
                    fmt::println("[{}] {} {}", getTokenTypeStr(item.type), item.pos, FORMAT_ARG(utf8::utf16to8(item.content)));
                }
            }
        } catch (const std::exception &e) {
            Profile::printAndClear(e);
            FAIL();
        }
    }

    /**
     * 测试程序是否可以正常运行
     */
    [[maybe_unused]] void testParse(const std::filesystem::path &cpackPath,
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
        for (const auto &command: commands) {
            try {
                std::chrono::high_resolution_clock::time_point startParse, endParse,
                        startDescription, endDescription,
                        startErrorReasons, endErrorReasons,
                        startSuggestions, endSuggestions,
                        startStructure, endStructure;
                startParse = std::chrono::high_resolution_clock::now();
                core->onTextChanged(command, command.length());
                endParse = std::chrono::high_resolution_clock::now();
                startDescription = std::chrono::high_resolution_clock::now();
                auto description = core->getDescription();
                endDescription = std::chrono::high_resolution_clock::now();
                startErrorReasons = std::chrono::high_resolution_clock::now();
                auto errorReasons = core->getErrorReasons();
                endErrorReasons = std::chrono::high_resolution_clock::now();
                startSuggestions = std::chrono::high_resolution_clock::now();
                auto suggestions = core->getSuggestions();
                endSuggestions = std::chrono::high_resolution_clock::now();
                startStructure = std::chrono::high_resolution_clock::now();
                auto structure = core->getStructure();
                endStructure = std::chrono::high_resolution_clock::now();
                fmt::println("parse: {}", FORMAT_ARG(utf8::utf16to8(command)));
                fmt::println("parse in {}", FORMAT_ARG(std::chrono::duration_cast<std::chrono::milliseconds>(endParse - startParse)));
                fmt::println("get description in {}", FORMAT_ARG(std::chrono::duration_cast<std::chrono::milliseconds>(endDescription - startDescription)));
                fmt::println("get error in {}", FORMAT_ARG(std::chrono::duration_cast<std::chrono::milliseconds>(endErrorReasons - startErrorReasons)));
                fmt::println("get suggestions in {}", FORMAT_ARG(std::chrono::duration_cast<std::chrono::milliseconds>(endSuggestions - startSuggestions)));
                fmt::println("get structure in {}", FORMAT_ARG(std::chrono::duration_cast<std::chrono::milliseconds>(endStructure - startStructure)));
                fmt::println("structure: {}", utf8::utf16to8(structure));
                fmt::println("description: {}", utf8::utf16to8(description));
                if (errorReasons.empty()) {
                    fmt::println("no error");
                } else {
                    fmt::println("error reasons:");
                    int i = 0;
                    for (const auto &errorReason: errorReasons) {
                        fmt::print("{}. {} {}\n{}{}{}\n",
                                   ++i,
                                   fmt::styled(utf8::utf16to8(command.substr(errorReason->start, errorReason->end - errorReason->start)), fg(fmt::color::red)),
                                   fmt::styled(utf8::utf16to8(errorReason->errorReason), fg(fmt::color::cornflower_blue)),
                                   utf8::utf16to8(command.substr(0, errorReason->start)),
                                   fmt::styled(errorReason->start == errorReason->end ? "~" : utf8::utf16to8(command.substr(errorReason->start, errorReason->end - errorReason->start)), fg(fmt::color::red)),
                                   utf8::utf16to8(command.substr((errorReason->end))));
                    }
                }
                if (suggestions->empty()) {
                    fmt::println("no suggestion");
                } else {
                    fmt::println("{} suggestions:", suggestions->size());
                    int i = 0;
                    for (const auto &item: *suggestions) {
                        if (i == 30) {
                            fmt::println("...");
                            break;
                        }
                        fmt::println("{}. {} {}",
                                     ++i,
                                     fmt::styled(utf8::utf16to8(item.content->name), fg(fmt::color::lime_green)),
                                     fmt::styled(utf8::utf16to8(item.content->description.value_or(u"")), fg(fmt::color::cornflower_blue)));
                        std::u16string result = command.substr(0, item.start)
                                                        .append(item.content->name)
                                                        .append(command.substr(item.end));
                        std::u16string greenPart = item.content->name;
                        if (item.end == command.length()) {
                            ASTNode astNode = Parser::parse(result, core->getCPack());
                            if (item.isAddSpace && astNode.isAllSpaceError()) {
                                greenPart.push_back(u' ');
                            }
                        }
                        fmt::println("{}{}{}",
                                     utf8::utf16to8(command.substr(0, item.start)),
                                     fmt::styled(utf8::utf16to8(greenPart), fg(fmt::color::lime_green)),
                                     utf8::utf16to8(command.substr(item.end)));
                    }
                }
                fmt::print("\n");
            } catch (const std::exception &e) {
                flag = true;
                CHelper::Profile::printAndClear(e);
            }
        }
        if (HEDLEY_UNLIKELY(flag)) {
            FAIL();
        }
    }

}// namespace CHelper::Test

TEST(MainTest, LexCommand) {
    CHelper::Test::testLex(
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

TEST(MainTest, ParseCommand) {
    std::filesystem::path resourceDir(RESOURCE_DIR);
    CHelper::Test::testParse(
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
