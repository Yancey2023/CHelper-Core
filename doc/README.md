There should be a file named `manifest.json` in the root directory of the pack:

```json
{
  "name": "example pack",
  "getDescription": "CHelper扩展包的官方示例",
  "minecraftVersion": "1.20",
  "author": "Yancey",
  "updateDate": "2023-11-7",
  "packID": "ExamplePack-1.20",
  "requiredPack": [
    "BasicPack-1.20"
  ],
  "version": 1,
  "isBasicPack": true
}
```

|       name       |  type   |       meaning        |                          markup                           |      is must have      |
|:----------------:|:-------:|:--------------------:|:---------------------------------------------------------:|:----------------------:|
|       name       | string  |      pack name       |                        for showing                        |    no(hope to have)    |
|   getDescription    | string  |   pack getDescription   |                        for showing                        |           no           |
| minecraftVersion | string  |  minecraft version   |                        for showing                        |           no           |
|      author      | string  |        author        |                        for showing                        |           no           |
|    updateDate    | string  |  latest update date  |                        for showing                        |           no           |
|      packId      | string  |       pack ID        |                   for recognizing pack                    |          yes           |
|   requiredPack   |  array  | IDs of required pack |               for recognizing dependencies                | no(default is nothing) |
|     version      | integer |     version code     |                  for recognizing version                  |          yes           |
|   isBasicPack    | boolean | if it is basic pack  | only one basic pack and more expend pack at the same time |  no(default is false)  |
