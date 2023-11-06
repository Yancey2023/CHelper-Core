cmake_minimum_required(VERSION 3.26)
project(CHelper)

set(CMAKE_CXX_STANDARD 17)

add_executable(CHelper main.cpp
        core/lexer/Token.cpp
        core/lexer/Token.h
        core/lexer/Pos.cpp
        core/lexer/Pos.h
        core/lexer/Lexer.cpp
        core/lexer/Lexer.h
        core/lexer/StringReader.cpp
        core/lexer/StringReader.h
        main.h
        core/parser/Parser.cpp
        core/parser/Parser.h)
