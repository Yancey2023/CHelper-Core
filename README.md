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
- [ ] checker, find id error through AST three and cpack,
- [ ] suggester, find id error through AST three and cpack,
- [ ] ...

## Why choosing c++

Because I am leaning c++ now, I want to improve myself through the project.

## Dependencies

|                 project                  |     description     |                                 license                                  |
|:----------------------------------------:|:-------------------:|:------------------------------------------------------------------------:|
| [json](https://github.com/nlohmann/json) | JSON for Modern C++ | [MIT license](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT) |
