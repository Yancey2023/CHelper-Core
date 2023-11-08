cmake_minimum_required(VERSION 3.26)
project(CHelper)

include_directories(include)

add_executable(CHelper
        main.cpp
        main.h
        core/lexer/Token.cpp
        core/lexer/Token.h
        core/lexer/Pos.cpp
        core/lexer/Pos.h
        core/lexer/Lexer.cpp
        core/lexer/Lexer.h
        core/lexer/StringReader.cpp
        core/lexer/StringReader.h
        core/parser/Parser.cpp
        core/parser/Parser.h
        core/resources/Manifest.cpp
        core/resources/Manifest.h
        core/resources/CPack.cpp
        core/resources/CPack.h
        core/resources/id/NormalId.cpp
        core/resources/id/NormalId.h
        core/util/JsonUtil.cpp
        core/util/JsonUtil.h
        core/resources/id/BlockId.cpp
        core/resources/id/BlockId.h
        core/resources/id/ItemId.cpp
        core/resources/id/ItemId.h
        )