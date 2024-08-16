from os import path

buildDirs = ['cmake-build-emscripten-release', 'cmake-build-emscripten-minsizerel']


def insert(content, before, end, string):
    return content.replace(before + end, before + string + end)


for buildDir in buildDirs:
    with open(path.join(buildDir, 'libCHelperWeb.js'), 'r') as fp:
        content = fp.read()
    insert(
        content,
        '',
        'function createWasm()',
        'export var wasmInitFuture = undefined;'
    )
    insert(content,
           'if(Module["instantiateWasm"]){try{return Module["instantiateWasm"](info,receiveInstance)}catch(e){err(`Module.instantiateWasm callback failed with error: ${e}`);return false}}instantiateAsync(wasmBinary,wasmBinaryFile,info,receiveInstantiationResult);',
           'return{}',
           'wasmInitFuture = instantiateAsync(wasmBinary, wasmBinaryFile, info, receiveInstantiationResult);'
           )
    content += '''
export class CHelperCore {
    constructor(cpack) {
        const cpackPtr = Module._malloc(cpack.byteLength)
        Module.HEAP8.set(cpack, cpackPtr)
        this._corePtr = Module._init(cpackPtr, cpack.byteLength)
        Module._free(cpackPtr)
        if (this._corePtr === 0) {
            throw "fail to init CHelper core"
        }
    }

    release() {
        Module._release(this._corePtr)
        this._corePtr = 0
    }

    onTextChanged(content, index) {
        Module.ccall("onTextChanged", null, ["number", "string", "number"], [this._corePtr, content, index])
    }

    onSelectionChanged(index) {
        Module._onSelectionChanged(this._corePtr, index)
    }

    getStructure() {
        return Module.ccall("getStructure", "string", ["number"], [this._corePtr])
    }

    getDescription() {
        return Module.ccall("getDescription", "string", ["number"], [this._corePtr])
    }

    getErrorReason() {
        return Module.ccall("getErrorReason", "string", ["number"], [this._corePtr])
    }

    getSuggestionSize() {
        return Module._getSuggestionSize(this._corePtr)
    }

    getSuggestionTitle(which) {
        return Module.ccall("getSuggestionTitle", "string", ["number", "number"], [this._corePtr, which])
    }

    getSuggestionDescription(which) {
        return Module.ccall("getSuggestionDescription", null, ["number", "number"], [this._corePtr, which])
    }

    getStringAfterSuggestionClick(which) {
        return Module.ccall("getStringAfterSuggestionClick", "string", ["number"], [this._corePtr])
    }

    getSelectionAfterSuggestionClick(which) {
        return Module.ccall("getSelectionAfterSuggestionClick", "number", ["number"], [this._corePtr])
    }
}'''
    with open(path.join(buildDir, 'libCHelperWeb.js'), 'w') as fp:
        fp.write(content)
