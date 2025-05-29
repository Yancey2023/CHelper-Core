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
    "version": "1.20",
    "versionType": "release",
    "branch": "release",
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

|     name     |  type   |         meaning          |                                    Remarks                                     |            Required            |
|:------------:|:-------:|:------------------------:|:------------------------------------------------------------------------------:|:------------------------------:|
|     name     | string  |        pack name         |                                used for display                                | no (not recommended to ignore) |
| description  | string  |     pack description     |                                used for display                                |               no               |
|   version    | string  |    minecraft version     |                                used for display                                |               no               |
| versionType  | string  |  minecraft version type  |                                used for display                                |               no               |
|    branch    | string  | minecraft version branch |                                used for display                                |               no               |
|    author    | string  |          author          |                                used for display                                |               no               |
|  updateDate  | string  |    latest update date    |                                used for display                                |               no               |
|    packId    | string  |         pack ID          |                           used for recognizing pack                            |               no               |
| requiredPack |  array  |   IDs of required pack   |                       used for recognizing dependencies                        |  no(no default dependencies)   |
| versionCode  | integer |       version code       |                          used for recognizing version                          |              yes               |
| isBasicPack  | boolean |   if it is basic pack    | only one basic pack and multiple extension pack can be loaded at the same time |      no(default is false)      |

> Note: Extension pack is not currently supported. You only can use basic pack now.

I am currently too lazy to write the English document.

If you need, please refer to the [Chinese document](README_CN.md).

The English document will be updated after CHelper is finished.
