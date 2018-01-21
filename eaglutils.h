#ifndef EAGLEYE_EAGLUTILS_H
#define EAGLEYE_EAGLUTILS_H

#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <map>

#include <boost/program_options.hpp>

typedef unsigned char BYTE;

namespace EAGLEye
{
    enum FileType
    {
        LOCATION_BASE,
        LOCATION_FULL,
        LANGUAGE,
        GLOBAL,
        GENERIC_CHUNKED
    };

    static const std::map<std::string, int> gameMap = {
            {"MW05",   0x2},
            {"UG2",    0x4},
            {"CARBON", 0x5},
            {"WORLD",  0x5a}
    };

    template <typename V>
    std::vector<std::string> extract_keys(std::map<std::string, V> const &input_map)
    {
        std::vector<std::string> retval;
        for (auto const &element : input_map)
        {
            retval.push_back(element.first);
        }
        return retval;
    }

    template<typename Base, typename T>
    inline bool instanceof(const T *ptr)
    {
        return dynamic_cast<const Base *>(ptr) != nullptr;
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

    template<typename T>
    T swap_endian(T u)
    {
        static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

        union
        {
            T u;
            unsigned char u8[sizeof(T)];
        } source, dest;

        source.u = u;

        for (size_t k = 0; k < sizeof(T); k++)
            dest.u8[k] = source.u8[sizeof(T) - k - 1];

        return dest.u;
    }

    template<typename Data>
    size_t readGeneric(std::ifstream &stream, Data &data, size_t size = sizeof(Data))
    {
        stream.read((char *) &data, size);

        return size;
    }

    void dumpBytes(std::ifstream &stream, size_t bytes = 0);
}

#endif //EAGLEYE_EAGLUTILS_H
