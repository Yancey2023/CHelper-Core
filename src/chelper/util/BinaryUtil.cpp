//
// Created by Yancey on 24-5-10.
//

#include "BinaryUtil.h"
#include "hedley.h"

namespace CHelper {

    BinaryWriter::BinaryWriter(bool isTargetSmallEndian,
                               std::ostream &ostream)
        : ostream(ostream) {
        bool isUsingSmallEndian;
        union {
            int32_t int32 = 0x01020304;
            int8_t int8;
        } data;
        if (data.int8 == 0x04) {
            isUsingSmallEndian = true;
        } else if (data.int8 == 0x01) {
            isUsingSmallEndian = false;
        } else {
            throw std::runtime_error("unknown byte order");
        }
        isNeedConvert = isUsingSmallEndian != isTargetSmallEndian;
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
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), sizeof(uint8_t));
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
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), sizeof(uint8_t));
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
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), sizeof(uint8_t));
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
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), sizeof(uint8_t));
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
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[7]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[6]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[5]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[4]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), sizeof(uint8_t));
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
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[7]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[6]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[5]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[4]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), sizeof(uint8_t));
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
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), sizeof(uint8_t));
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
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[7]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[6]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[5]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[4]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[3]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[2]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[1]), sizeof(uint8_t));
            ostream.write(reinterpret_cast<const char *>(&targetNum.uint8[0]), sizeof(uint8_t));
        } else {
            ostream.write(reinterpret_cast<const char *>(&t), sizeof(t));
        }
    }

    void BinaryWriter::encodeSize(size_t t) {
        if (t > std::numeric_limits<uint16_t>::max()) {
            throw std::runtime_error("fail to write size_t because size is too big");
        }
        encode(static_cast<uint16_t>(t));
    }

    void BinaryWriter::encode(const std::optional<bool> &t) {
        if (HEDLEY_LIKELY(t.has_value())) {
            encode(t.value());
        } else {
            encode((uint8_t) 0xFF);
        }
    }

    void BinaryWriter::encode(const std::string &t) {
        encodeSize(t.length());
        if (!t.empty()) {
            ostream.write(t.data(), t.length());
        }
    }

    void BinaryWriter::encode(const std::optional<std::string> &t) {
        if (HEDLEY_LIKELY(t.has_value() && !t->empty())) {
            encode(t.value());
        } else {
            encodeSize(0);
        }
    }

    void BinaryWriter::encode(const std::optional<int32_t> &t) {
        if (HEDLEY_LIKELY(t.has_value())) {
            if (HEDLEY_UNLIKELY(t.value() == std::numeric_limits<int32_t>::min())) {
                throw std::runtime_error("fail to encode std::optional<int32_t>");
            }
            encode(t.value());
        } else {
            encode(std::numeric_limits<int32_t>::min());
        }
    }

    BinaryReader::BinaryReader(bool isTargetSmallEndian,
                               std::istream &istream)
        : istream(istream) {
        bool isUsingSmallEndian;
        union {
            int32_t int32 = 0x01020304;
            int8_t int8;
        } data;
        if (data.int8 == 0x04) {
            isUsingSmallEndian = true;
        } else if (data.int8 == 0x01) {
            isUsingSmallEndian = false;
        } else {
            throw std::runtime_error("unknown byte order");
        }
        isNeedConvert = isUsingSmallEndian != isTargetSmallEndian;
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
            } result;
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), sizeof(uint8_t));
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
            } result;
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), sizeof(uint8_t));
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
            } result;
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), sizeof(uint8_t));
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
            } result;
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), sizeof(uint8_t));
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
            } result;
            istream.read(reinterpret_cast<char *>(&result.uint8[7]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[6]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[5]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[4]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), sizeof(uint8_t));
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
            } result;
            istream.read(reinterpret_cast<char *>(&result.uint8[7]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[6]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[5]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[4]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), sizeof(uint8_t));
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
            } result;
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), sizeof(uint8_t));
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
            } result;
            istream.read(reinterpret_cast<char *>(&result.uint8[7]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[6]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[5]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[4]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[3]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[2]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[1]), sizeof(uint8_t));
            istream.read(reinterpret_cast<char *>(&result.uint8[0]), sizeof(uint8_t));
            t = result.num;
        } else {
            istream.read(reinterpret_cast<char *>(&t), sizeof(t));
        }
    }

    void BinaryReader::decode(std::optional<bool> &t) {
        switch (read<uint8_t>()) {
            case 0:
                t = false;
                break;
            case 1:
                t = true;
                break;
            default:
                t = std::nullopt;
                break;
        }
    }

    void BinaryReader::decode(std::string &t) {
        size_t length = readSize();
        if (HEDLEY_UNLIKELY(length == 0)) {
            t = {};
        }
        std::vector<char> buffer(length);
        istream.read(buffer.data(), static_cast<std::streamsize>(sizeof(char) * length));
        t = {buffer.begin(), buffer.end()};
    }

    void BinaryReader::decode(std::optional<std::string> &t) {
        size_t length = readSize();
        if (HEDLEY_UNLIKELY(length == 0)) {
            t = std::nullopt;
            return;
        }
        std::vector<char> buffer(length);
        istream.read(buffer.data(), static_cast<std::streamsize>(sizeof(char) * length));
        t = {{buffer.begin(), buffer.end()}};
    }

    void BinaryReader::decode(std::optional<int32_t> &t) {
        int32_t value;
        decode(value);
        if (value == std::numeric_limits<int32_t>::min()) {
            t = std::nullopt;
        } else {
            t = value;
        }
    }

}// namespace CHelper