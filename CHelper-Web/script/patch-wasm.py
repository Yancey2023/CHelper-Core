from os import path

buildDirs = ['cmake-build-emscripten-release', 'cmake-build-emscripten-minsizerel']


def insert(content: str, before: str, end: str, string: str) -> str:
    return content.replace(before + end, before + string + end)


for buildDir in buildDirs:
    with open(path.join(buildDir, 'libCHelperWeb.js'), 'r') as fp:
        content = fp.read()
    content = insert(
        content,
        'var wasmExports;',
        'createWasm();',
        'export var createWasmFuture = '
    )
    content = content.replace('\n', '')
    content += ('''
var UTF16ToString = (ptr) => {
    let str = '';
    let i = ptr >> 1;
    while (true) {
        const codeUnit = HEAPU16[i];
        if (codeUnit === 0) break;
        str += String.fromCharCode(codeUnit);
        ++i;
    }
    return str;
};

var stringToUTF16 = (str) => {
    const outPtr = _malloc((str.length + 1) * 2);
    var start = outPtr / 2;
    var end  = start + str.length;
    var i = start;
    while (i < end) {
        HEAPU16[i] = str.charCodeAt(i - start);
        ++i;
    }
    HEAPU16[i] = 0;
    return outPtr;
};

export class CHelperCore {
    constructor(cpack) {
        const cpackPtr = _malloc(cpack.byteLength);
        HEAP8.set(cpack, cpackPtr);
        this._corePtr = _init(cpackPtr, cpack.byteLength);
        _free(cpackPtr);
        if (this._corePtr === 0) {
            throw "fail to init CHelper core";
        }
    }

    release() {
        _release(this._corePtr);
        this._corePtr = 0;
    }

    onTextChanged(content, index) {
        const contentPtr = stringToUTF16(content);
        _onTextChanged(this._corePtr, contentPtr, index);
        _free(contentPtr);
    }

    onSelectionChanged(index) {
        _onSelectionChanged(this._corePtr, index);
    }

    getStructure() {
        return UTF16ToString(_getStructure(this._corePtr));
    }

    getDescription() {
        return UTF16ToString(_getDescription(this._corePtr));
    }

    getErrorReason() {
        return UTF16ToString(_getErrorReason(this._corePtr));
    }

    getSuggestionSize() {
        return _getSuggestionSize(this._corePtr);
    }

    getSuggestionTitle(which) {
        return UTF16ToString(_getSuggestionTitle(this._corePtr, which));
    }

    getSuggestionDescription(which) {
        return UTF16ToString(_getSuggestionDescription(this._corePtr, which));
    }

    onSuggestionClick(which) {
        _onSuggestionClick(this._corePtr, which);
    }

    getStringAfterSuggestionClick() {
        return UTF16ToString(_getStringAfterSuggestionClick(this._corePtr));
    }

    getSelectionAfterSuggestionClick() {
        return UTF16ToString(_getSelectionAfterSuggestionClick(this._corePtr));
    }
}'''
                .replace(' ', '')
                .replace('\n', '')
                .replace('const', 'const ')
                .replace('let', 'var ')
                .replace('var', 'var ')
                .replace('return', 'return ')
                .replace('exportclassCHelperCore', 'export class CHelperCore')
                .replace('failtoinitCHelpercore', 'fail to init CHelper core')
                .replace('const ructor', 'constructor')
                )
    with open(path.join(buildDir, 'libCHelperWeb.js'), 'w') as fp:
        fp.write(content)
