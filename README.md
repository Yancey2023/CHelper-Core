# CHelper

**This project is unfinished, so it can't work.**

---

## Description

I want to make an Android command IDE for minecraft bedrock version , so here is CHelper, which means Command Helper.

## Plan

- [x] **Lexer**: transforms command strings into a token list
- [x] **CPack**: a resource pack containing IDs and commands
- [x] **Parser**: builds an Abstract Syntax Tree (AST) and identifies structural errors
- [x] **Description**: retrieves the description of the currently written node
- [x] **ID Checker**: finds ID errors based on the AST
- [x] **Suggestion Getter**: provides completion suggestions for IDs based on the AST
- [x] **Structure Retriever**: constructs a command structure string
- [x] **Target Selector**: target selector command param
- [ ] **`execute` command support**: support a command named `execute`
- [ ] **json support**: json command param
- [ ] **block state support**: block states command param
- [ ] **highlight command **: obtains the highlight color for displayed commands

## Why choosing c++

Because I am leaning c++ now. I want to improve myself through this project.

## Dependencies

|                 project                  |     description     |                                 license                                  |
|:----------------------------------------:|:-------------------:|:------------------------------------------------------------------------:|
| [json](https://github.com/nlohmann/json) | JSON for Modern C++ | [MIT license](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT) |

## Special Thanks

- JetBrains License for Open Source Development

![JetBrains Logo (Main) logo](https://resources.jetbrains.com/storage/products/company/brand/logos/jb_beam.svg)
