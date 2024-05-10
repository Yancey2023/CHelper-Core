//
// Created by Yancey on 24-5-10.
//

#ifndef CHELPER_BINWRITER_H
#define CHELPER_BINWRITER_H

namespace CHelper {

    class BinWriter {
    public:
        std::ostream ostream;

        template<class T>
        void write(T content) {
            write(content);
        }

    private:
        void writeInt8(int8_t num);

        void writeUInt8(uint8_t num);

        void writeInt32(int32_t num);

        void writeUInt32(uint32_t num);

        void writeInt64(int64_t num);

        void writeUInt64(uint64_t num);

        void writeFloat(float num);

        void writeDouble(double num);

        void writeString(std::string str);

    };

} // CHelper

#endif //CHELPER_BINWRITER_H
