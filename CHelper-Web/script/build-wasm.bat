@REM 加载环境（假设已经下载好了emsdk并且加进了环境变量）
echo ----- 加载环境 -----
call emsdk_env.bat
call cd ..

@REM 编译Release模式
echo ----- [Release] 加载CMake项目 -----
call emcmake cmake -B cmake-build-emscripten-release -DCMAKE_BUILD_TYPE=Release
call cd cmake-build-emscripten-release
echo ----- [Release] 编译CMake项目 -----
call emmake make -j
echo ----- [Release] 链接wasm -----
call emcc libCHelperWeb.a include/fmt/libfmt.a -O3 -o libCHelperWeb.js -s FILESYSTEM=0 -s DISABLE_EXCEPTION_CATCHING=1 -s ALLOW_MEMORY_GROWTH -s ENVIRONMENT="web" -s EXPORTED_FUNCTIONS="['_init','_release','_onTextChanged','_onSelectionChanged','_getDescription','_getErrorReason','_getSuggestionSize','_getSuggestionTitle','_getSuggestionDescription', '_onSuggestionClick', '_malloc', '_free']" -s WASM=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']"
call cd ..

@REM 编译MinSizeRel模式
echo ----- [MinSizeRel] 加载CMake项目 -----
call emcmake cmake -B cmake-build-emscripten-minsizerel -DCMAKE_BUILD_TYPE=MinSizeRel
call cd cmake-build-emscripten-minsizerel
echo ----- [MinSizeRel] 编译CMake项目 -----
call emmake make -j
echo ----- [MinSizeRel] 链接wasm -----
call emcc libCHelperWeb.a include/fmt/libfmt.a -Os -o libCHelperWeb.js -s FILESYSTEM=0 -s DISABLE_EXCEPTION_CATCHING=1 -s ALLOW_MEMORY_GROWTH -s ENVIRONMENT="web" -s EXPORTED_FUNCTIONS="['_init','_release','_onTextChanged','_onSelectionChanged','_getDescription','_getErrorReason','_getSuggestionSize','_getSuggestionTitle','_getSuggestionDescription', '_onSuggestionClick', '_malloc', '_free']" -s WASM=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']"
call cd ..

@REM 修改JS胶水代码
echo ----- 修改JS胶水代码 -----
call python ./script/patch-wasm.py

@REM 暂停
call pause
