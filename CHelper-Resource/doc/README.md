English | [简体中文](README_CN.md)

# CPack document

Cpack(Command Pack) is the resource pack of CHelper(Command Helper), which is a command helper for Minecraft Bedrock
Edition.

## manifest.json

There should be a file named `manifest.json` in the root directory of the pack:

```json
{
  "name": "example pack",
  "description": "CHelper official extension pack",
  "minecraftVersion": "1.20",
  "author": "Yancey",
  "updateDate": "2023-11-7",
  "packID": "ExamplePack-1.20",
  "requiredPack": [
    "BasicPack-1.20"
  ],
  "versionCode": 1,
  "isBasicPack": true
}
```

|       name       |  type   |       meaning        |                                    Remarks                                     |            Required            |
|:----------------:|:-------:|:--------------------:|:------------------------------------------------------------------------------:|:------------------------------:|
|       name       | string  |      pack name       |                                Used for display                                | No (not recommended to ignore) |
|   description    | string  |   pack description   |                                Used for display                                |               No               |
| minecraftVersion | string  |  minecraft version   |                                Used for display                                |               No               |
|      author      | string  |        author        |                                Used for display                                |               No               |
|    updateDate    | string  |  latest update date  |                                Used for display                                |               No               |
|      packId      | string  |       pack ID        |                           Used for recognizing pack                            |               No               |
|   requiredPack   |  array  | IDs of required pack |                       Used for recognizing dependencies                        |  No(no default dependencies)   |
|   versionCode    | integer |     version code     |                          Used for recognizing version                          |              Yes               |
|   isBasicPack    | boolean | if it is basic pack  | only one basic pack and multiple extension pack can be loaded at the same time |      No(default is false)      |

> Note: Extension pack is not currently supported. You only can use basic pack now.

I am currently too lazy to write the English document.

If you need, please refer to the [Chinese document](README_CN.md).

The English document will be updated after CHelper is finished.
