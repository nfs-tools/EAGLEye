#ifndef EAGLEYE_EAGLUTILS_H
#define EAGLEYE_EAGLUTILS_H

#include "eagldefines.h"
#include "eagldata.h"

#include <fstream>
#include <cassert>

#include <boost/format.hpp>

namespace EAGLEye
{
    template<typename T>
    size_t readGeneric(std::ifstream &stream, T &data, size_t size = sizeof(T))
    {
        stream.read((char *) &data, size);

        return size;
    }

    template<typename T>
    size_t readVector(std::ifstream &stream, std::vector<T> &data, size_t size)
    {
        stream.read((char *) &data[0], size);

        return size;
    }

    template<typename Data>
    size_t readGenericArray(std::ifstream &stream, Data data[], size_t size)
    {
        stream.read((char *) &data[0], size);

        return size;
    }

    template<typename Data>
    size_t writeGeneric(std::ofstream &stream, Data data, size_t size = sizeof(Data))
    {
        stream.write((const char *) &data, size);

        return size;
    }

    template<typename K, typename V>
    std::vector<K> extract_keys(std::map<K, V> const &input_map)
    {
        std::vector<K> retVec;
        for (auto const &element : input_map)
        {
            retVec.push_back(element.first);
        }
        return retVec;
    }

    template<typename Base, typename T>
    inline bool instanceOf(const T *ptr)
    {
        return dynamic_cast<const Base *>(ptr) != nullptr;
    }

    template<typename Data>
    void hexdump(std::ofstream &stream, Data *data, size_t len = sizeof(Data))
    {
        unsigned int i;
        unsigned int r, c;

        if (data == nullptr)
            return;

        for (r = 0, i = 0; r < (len / 16 + static_cast<unsigned int>(len % 16 != 0)); r++, i += 16)
        {
            stream << boost::format("%04X:    ") % i;

            for (c = i; c < i + 8; c++) /* left half of hex dump */
                if (c < len)
                    stream << boost::format("%02X ") % static_cast<int>(((unsigned char const *) data)[c]);
                else
                    stream << "   ";

            stream << "  ";

            for (c = i + 8; c < i + 16; c++) /* right half of hex dump */
                if (c < len)
                    stream << boost::format("%02X ") % static_cast<int>(((unsigned char const *) data)[c]);
                else
                    stream << "   "; /* pad if short line */

            stream << "  ";

            for (c = i; c < i + 16; c++) /* ASCII dump */
                if (c < len)
                    if (((unsigned char const *) data)[c] >= 32 &&
                        ((unsigned char const *) data)[c] < 127)
                        stream << boost::format("%c") % ((char const *) data)[c];
                    else
                        stream << ".";
                else
                    stream << " ";

            stream << std::endl;
        }
    }

    template<typename Data>
    void hexdump(FILE *stream, Data *data, size_t len = sizeof(Data))
    {
        unsigned int i;
        unsigned int r, c;

        if (stream == nullptr)
            return;
        if (data == nullptr)
            return;

        for (r = 0, i = 0; r < (len / 16 + static_cast<unsigned int>(len % 16 != 0)); r++, i += 16)
        {
            fprintf(stream, "%04X:   ", i); /* location of first byte in line */

            for (c = i; c < i + 8; c++) /* left half of hex dump */
                if (c < len)
                    fprintf(stream, "%02X ", ((unsigned char const *) data)[c]);
                else
                    fprintf(stream, "   "); /* pad if short line */

            fprintf(stream, "  ");

            for (c = i + 8; c < i + 16; c++) /* right half of hex dump */
                if (c < len)
                    fprintf(stream, "%02X ", ((unsigned char const *) data)[c]);
                else
                    fprintf(stream, "   "); /* pad if short line */

            fprintf(stream, "   ");

            for (c = i; c < i + 16; c++) /* ASCII dump */
                if (c < len)
                    if (((unsigned char const *) data)[c] >= 32 &&
                        ((unsigned char const *) data)[c] < 127)
                        fprintf(stream, "%c", ((char const *) data)[c]);
                    else
                        fprintf(stream, "."); /* put this for non-printables */
                else
                    fprintf(stream, " "); /* pad if short line */

            fprintf(stream, "\n");
        }

        fflush(stream);
    }

    BYTE readByte(std::ifstream &stream);

    void dumpBytes(std::ifstream &stream, size_t bytes = 0);

    long getFileLength(std::ifstream &stream);
}

#endif //EAGLEYE_EAGLUTILS_H