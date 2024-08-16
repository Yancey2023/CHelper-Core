cd ..

git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk/emsdk_env.sh
cd ..

emcmake cmake -B cmake-build-emscripten-release -DCMAKE_BUILD_TYPE=Release --target CHelperWeb
cd cmake-build-emscripten-release
emmake make -j
emcc libCHelperWeb.a include/fmt/libfmt.a -O3 -o libCHelperWeb.js -s FILESYSTEM=0 -s DISABLE_EXCEPTION_CATCHING=1 -s ALLOW_MEMORY_GROWTH -s ENVIRONMENT="web" -s EXPORTED_FUNCTIONS="['_init','_release','_onTextChanged','_onSelectionChanged','_getDescription','_getErrorReason','_getSuggestionSize','_getSuggestionTitle','_getSuggestionDescription', '_onSuggestionClick', '_getStringAfterSuggestionClick', '_getSelectionAfterSuggestionClick', '_malloc', '_free']" -s WASM=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']"
cd..

emcmake cmake -B cmake-build-emscripten-minsizerel -DCMAKE_BUILD_TYPE=MinSizeRel --target CHelperWeb
cd cmake-build-emscripten-minsizerel
emmake make -j
emcc libCHelperWeb.a include/fmt/libfmt.a -Os -o libCHelperWeb.js -s FILESYSTEM=0 -s DISABLE_EXCEPTION_CATCHING=1 -s ALLOW_MEMORY_GROWTH -s ENVIRONMENT="web" -s EXPORTED_FUNCTIONS="['_init','_release','_onTextChanged','_onSelectionChanged','_getDescription','_getErrorReason','_getSuggestionSize','_getSuggestionTitle','_getSuggestionDescription', '_onSuggestionClick', '_getStringAfterSuggestionClick', '_getSelectionAfterSuggestionClick', '_malloc', '_free']" -s WASM=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']"
cd..

python ./script/wasm-patch.py