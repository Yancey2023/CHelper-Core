//
// Created by Yancey on 24-5-10.
//

#include "BinaryUtil.h"
#include "hedley.h"

namespace CHelper {

    bool getIsNeedConvert(bool isTargetSmallEndian) {
        bool isUsingSmallEndian;
        union {
            int32_t int32 = 0x01020304;
            int8_t int8;
        } data;
        if (HEDLEY_UNLIKELY(data.int8 == 0x04)) {
            isUsingSmallEndian = true;
        } else if (HEDLEY_UNLIKELY(data.int8 == 0x01)) {
            isUsingSmallEndian = false;
        } else {
            throw std::runtime_error("unknown byte order");
        }
        return isUsingSmallEndian != isTargetSmallEndian;
    }

    BinaryWriter::BinaryWriter(bool isTargetSmallEndian,
                               std::ostream &ostream)
        : ostream(ostream) {
        isNeedConvert = getIsNeedConvert(isTargetSmallEndian);
    }

    void BinaryWriter::encode(bool t) {
        if (t) {
            encode((uint8_t) 1);
        } else {
            encode((uint8_t) 0);
        }
    }

    void BinaryWriter::encode(int8_t t) {
        ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
    }

    void BinaryWriter::encode(uint8_t t) {
        ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
    }

    void BinaryWriter::encode(int16_t t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                int16_t num;
                uint8_t uint8[2];
            } targetNum = {t};
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), 1);
        } else {
            ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
        }
    }

    void BinaryWriter::encode(uint16_t t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                uint16_t num;
                uint8_t uint8[2];
            } targetNum = {t};
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), 1);
        } else {
            ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
        }
    }

    void BinaryWriter::encode(int32_t t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                int32_t num;
                uint8_t uint8[4];
            } targetNum = {t};
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), 1);
        } else {
            ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
        }
    }

    void BinaryWriter::encode(uint32_t t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                uint32_t num;
                uint8_t uint8[4];
            } targetNum = {t};
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), 1);
        } else {
            ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
        }
    }

    void BinaryWriter::encode(int64_t t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                int64_t num;
                uint8_t uint8[8];
            } targetNum = {t};
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[7]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[6]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[5]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[4]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), 1);
        } else {
            ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
        }
    }

    void BinaryWriter::encode(uint64_t t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                uint64_t num;
                uint8_t uint8[8];
            } targetNum = {t};
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[7]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[6]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[5]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[4]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), 1);
        } else {
            ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
        }
    }

    void BinaryWriter::encode(float t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                float num;
                uint8_t uint8[4];
            } targetNum = {t};
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), 1);
        } else {
            ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
        }
    }

    void BinaryWriter::encode(double t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                double num;
                uint8_t uint8[8];
            } targetNum = {t};
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[7]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[6]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[5]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[4]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), 1);
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), 1);
        } else {
            ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
        }
    }

    void BinaryWriter::encodeSize(size_t t) {
        if (HEDLEY_UNLIKELY(t > std::numeric_limits<uint16_t>::max())) {
            throw std::runtime_error("fail to write size_t because size is too big");
        }
        encode(static_cast<uint16_t>(t));
    }

    void BinaryWriter::encode(const std::string &t) {
        encodeSize(t.length());
        if (HEDLEY_UNLIKELY(!t.empty())) {
            ostream.write(t.data(), static_cast<std::streamsize>(t.length()));
        }
    }

    BinaryReader::BinaryReader(bool isTargetSmallEndian,
                               std::istream &istream)
        : istream(istream) {
        isNeedConvert = getIsNeedConvert(isTargetSmallEndian);
    }

    size_t BinaryReader::readSize() {
        return read<uint16_t>();
    }

    void BinaryReader::decode(bool &t) {
        t = read<uint8_t>() != 0;
    }

    void BinaryReader::decode(int8_t &t) {
        istream.read(reinterpret_cast<char *>(&t), sizeof(t));
    }

    void BinaryReader::decode(uint8_t &t) {
        istream.read(reinterpret_cast<char *>(&t), sizeof(t));
    }

    void BinaryReader::decode(int16_t &t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                int16_t num;
                uint8_t uint8[2];
            } result{};
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), 1);
            t = result.num;
        } else {
            istream.read(reinterpret_cast<char *>(&t), sizeof(t));
        }
    }

    void BinaryReader::decode(uint16_t &t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                uint16_t num;
                uint8_t uint8[2];
            } result{};
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), 1);
            t = result.num;
        } else {
            istream.read(reinterpret_cast<char *>(&t), sizeof(t));
        }
    }

    void BinaryReader::decode(int32_t &t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                int32_t num;
                uint8_t uint8[4];
            } result{};
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), 1);
            t = result.num;
        } else {
            istream.read(reinterpret_cast<char *>(&t), sizeof(t));
        }
    }

    void BinaryReader::decode(uint32_t &t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                uint32_t num;
                uint8_t uint8[4];
            } result{};
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), 1);
            t = result.num;
        } else {
            istream.read(reinterpret_cast<char *>(&t), sizeof(t));
        }
    }

    void BinaryReader::decode(int64_t &t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                int64_t num;
                uint8_t uint8[4];
            } result{};
            istream.read(reinterpret_cast<char *>(&result.uint8[7]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[6]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[5]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[4]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), 1);
            t = result.num;
        } else {
            istream.read(reinterpret_cast<char *>(&t), sizeof(t));
        }
    }

    void BinaryReader::decode(uint64_t &t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                uint64_t num;
                uint8_t uint8[4];
            } result{};
            istream.read(reinterpret_cast<char *>(&result.uint8[7]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[6]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[5]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[4]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), 1);
            t = result.num;
        } else {
            istream.read(reinterpret_cast<char *>(&t), sizeof(t));
        }
    }

    void BinaryReader::decode(float &t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                float num;
                uint8_t uint8[4];
            } result{};
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), 1);
            t = result.num;
        } else {
            istream.read(reinterpret_cast<char *>(&t), sizeof(t));
        }
    }

    void BinaryReader::decode(double &t) {
        if (HEDLEY_UNLIKELY(isNeedConvert)) {
            union {
                double num;
                uint8_t uint8[4];
            } result{};
            istream.read(reinterpret_cast<char *>(&result.uint8[7]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[6]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[5]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[4]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), 1);
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), 1);
            t = result.num;
        } else {
            istream.read(reinterpret_cast<char *>(&t), sizeof(t));
        }
    }

    void BinaryReader::decode(std::string &t) {
        size_t length = readSize();
        if (HEDLEY_UNLIKELY(length == 0)) {
            t.clear();
            return;
        }
        t.resize(length);
        istream.read(&t[0], static_cast<std::streamsize>(length));
    }

}// namespace CHelper