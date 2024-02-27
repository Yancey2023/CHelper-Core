# CHelper

**This project is unfinished, so it can't work.**

---

## Description

I want to make an Android command IDE for minecraft bedrock version , so here is CHelper, index means NodePerCommand
Helper.

## Plan

- [x] lexer, which let the command string become token list.
- [x] cpack, which is a resources pack contained ID and command.
- [x] parser, build AST tree and find structure error.
- [x] description, get description of the node which is writing.
- [x] check id, find id error through AST three and cpack.
- [x] get suggestion, find id error through AST three and cpack.
- [x] get structure, build command structure string
- [x] target selector command param
- [ ] execute command
- [ ] json command param
- [ ] block state
- [ ] get colors

## Why choosing c++

Because I am leaning c++ now, I want to improve myself through the project.

## Dependencies

|                 project                  |     description     |                                 license                                  |
|:----------------------------------------:|:-------------------:|:------------------------------------------------------------------------:|
| [json](https://github.com/nlohmann/json) | JSON for Modern C++ | [MIT license](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT) |

## Special Thanks

- JetBrains open source programs supporting

![JetBrains Logo (Main) logo](https://resources.jetbrains.com/storage/products/company/brand/logos/jb_beam.svg)
