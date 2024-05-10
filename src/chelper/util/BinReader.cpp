//
// Created by Yancey on 24-5-10.
//

#include "BinWriter.h"

namespace CHelper {

    void BinWriter::writeInt8(int8_t num) {
        ostream << num;
    }

    void BinWriter::writeUInt8(uint8_t num) {
        ostream << num;
    }

    void BinWriter::writeInt32(int32_t num) {
        ostream << num;
    }

    void BinWriter::writeUInt32(uint32_t num) {
        ostream << num;
    }

    void BinWriter::writeInt64(int64_t num) {
        ostream << num;
    }

    void BinWriter::writeUInt64(uint64_t num) {
        ostream << num;
    }

    void BinWriter::writeFloat(float num) {
        ostream << num;
    }

    void BinWriter::writeDouble(double num) {
        ostream << num;
    }

    void BinWriter::writeString(std::string str) {
        ostream << str;
    }

} // CHelper