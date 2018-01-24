#ifndef EAGLEYE_EAGLUTILS_H
#define EAGLEYE_EAGLUTILS_H

#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <map>

#include <boost/program_options.hpp>

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef unsigned char BYTE;

#define PACK __attribute__((__packed__))
#define BIN_ID(n) ((DWORD) (n) & 0xFFFFFF) // strips the high byte i guess

namespace EAGLEye
{
    struct PACK Point3D
    {
        float x, y, z, w;
    };

    union PACK Matrix
    {
        float m[16];
        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
    };

    enum FileType
    {
        NORMAL,
        COMPRESSED,
        LOCATION_BASE
    };

    static const std::map<std::string, int> gameMap = {
            {"MW05",   0x2},
            {"UG2",    0x4},
            {"CARBON", 0x5},
            {"WORLD",  0x5a}
    };

    static const std::map<int, std::string> chunkIdMap = {
            {0xb3300000, "BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS"},
            {0xb0300100, "BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS_ANIM"},
            {0x80134000, "BCHUNK_SPEED_ESOLID_LIST_CHUNKS"},
            {0x80034100, "BCHUNK_SPEED_SCENERY_SECTION"},
            {0x00034027, "BCHUNK_SPEED_SMOKEABLE_SPAWNER"},
            {0x00034110, "BCHUNK_TRACKSTREAMER_0"},
            {0x00034111, "BCHUNK_TRACKSTREAMER_1"},
            {0x00034112, "BCHUNK_TRACKSTREAMER_2"},
            {0x00034113, "BCHUNK_TRACKSTREAMER_3"},
            {0x00034107, "BCHUNK_TRACKSTREAMER_7"},
            {0x00037260, "BCHUNK_SPEED_BBGANIM_INSTANCE_TREE"},
            {0x00037250, "BCHUNK_SPEED_BBGANIM_INSTANCE_NODE"},
            {0x00037270, "BCHUNK_SPEED_BBGANIM_ENDPACKHEADER"},
            {0x80135000, "BCHUNK_SPEED_ELIGHT_CHUNKS"},
            {0x80036000, "BCHUNK_SPEED_EMTRIGGER_PACK"},
            {0x00037220, "BCHUNK_SPEED_BBGANIM_BLOCKHEADER"},
            {0x0003bc00, "BCHUNK_SPEED_EMITTER_LIBRARY"},
            {0x00030201, "BCHUNK_FENG_FONT"},
            {0x00030210, "BCHUNK_FENG_PACKAGE_COMPRESSED"},
            {0x00030203, "BCHUNK_FENG_PACKAGE"},
            {0x00135200, "BCHUNK_ELIGHTS"},
            {0x00034600, "BCHUNK_CARINFO_ARRAY"},
            {0x00034601, "BCHUNK_CARINFO_SKININFO"},
            {0x00034608, "BCHUNK_CARINFO_ANIMHOOKUPTABLE"},
            {0x00034609, "BCHUNK_CARINFO_ANIMHIDETABLES"},
            {0x00034607, "BCHUNK_CARINFO_SLOTTYPES"},
            {0x80034602, "BCHUNK_CARINFO_CARPART"},
            {0x00034201, "BCHUNK_TRACKINFO"},
            {0x00034202, "BCHUNK_SUN"},
            {0x80035000, "BCHUNK_ACIDFX"},
            {0x80035010, "BCHUNK_ACIDFX"},
            {0x00035021, "BCHUNK_ACIDFX"},
            {0x00035020, "BCHUNK_ACIDFX_EMITTER"},
            {0x00034b00, "BCHUNK_DIFFICULTYINFO"},
            {0x00034a07, "BCHUNK_STYLEMOMENTSINFO"},
            {0x00030220, "BCHUNK_FEPRESETCARS"},
            {0x00e34009, "BCHUNK_EAGLSKELETONS"},
            {0x00e34010, "BCHUNK_EAGLANIMATIONS"},
            {0x00039020, "BCHUNK_MOVIECATALOG"},
            {0x8003b900, "BCHUNK_BOUNDS"},
            {0x0003bd00, "BCHUNK_EMITTERSYSTEM_TEXTUREPAGE"},
            {0xb0300300, "BCHUNK_PCAWEIGHTS"},
            {0x30300201, "BCHUNK_COLORCUBE"},
            {0x80037050, "BCHUNK_ANIMDIRECTORYDATA"},
            {0x8003b200, "BCHUNK_ICECAMERASET"},
            {0x8003B201, "BCHUNK_ICECAMERASET"},
            {0x8003b202, "BCHUNK_ICECAMERASET"},
            {0x8003b203, "BCHUNK_ICECAMERASET"},
            {0x8003b500, "BCHUNK_SOUNDSTICHS"},
            {0x80034147, "BCHUNK_TRACKPATH"},
            {0x00034146, "BCHUNK_TRACKPOSITIONMARKERS"},
            {0x00034158, "BCHUNK_VISIBLESECTION"},
            {0x80034150, "BCHUNK_VISIBLESECTION"},
            {0x00034250, "BCHUNK_WEATHERMAN"},
            {0x8003b000, "BCHUNK_QUICKSPLINE"},
            {0x8003b600, "BCHUNK_PARAMETERMAPS"},
            {0x80034100, "BCHUNK_SPEED_SCENERY_SECTION"},
            {0x00034108, "BCHUNK_SCENERY"},
            {0x00034109, "BCHUNK_SCENERYGROUP"},
            {0x8003410b, "BCHUNK_SCENERY"},
            {0x0003b800, "BCHUNK_WWORLD"},
            {0x0003b801, "BCHUNK_CARP_WCOLLISIONPACK"},
            {0x8003b810, "BCHUNK_EVENTSEQUENCE"},
            {0x0003414d, "BCHUNK_TRACKPATH"},
            {0x00037080, "BCHUNK_WORLDANIMENTITYDATA"},
            {0x00037110, "BCHUNK_WORLDANIMTREEMARKER"},
            {0x00037150, "BCHUNK_WORLDANIMINSTANCEENTRY"},
            {0x00037090, "BCHUNK_WORLDANIMDIRECTORYDATA"},
            {0x30300200, "BCHUNK_DDSTEXTURE"},
            {0x0003ce12, "BCHUNK_SKINREGIONDATABASE"},
            {0x0003ce13, "BCHUNK_VINYLMETADATA"},
            {0x0003b200, "BCHUNK_ICECAMERAS"},
            {0x00039000, "BCHUNK_LANGUAGE"},
            {0x00039001, "BCHUNK_LANGUAGEHISTOGRAM"},
            {0x00034a08, "BCHUNK_STYLEREWARDCHUNK"},
            {0x00030230, "BCHUNK_MAGAZINES"},
            {0x00034026, "BCHUNK_SMOKEABLES"},
            {0x00034492, "BCHUNK_CAMERA"},
            {0x80034405, "BCHUNK_CAMERA"},
            {0x80034425, "BCHUNK_CAMERA"},
            {0x80034410, "BCHUNK_CAMERA"},
            {0x80034415, "BCHUNK_CAMERA"},
            {0x80034420, "BCHUNK_CAMERA"},
            {0x0003a000, "BCHUNK_ELIPSE_TABLE"},
            {0x00034036, "BCHUNK_NIS_SCENE_MAPPER_DATA"},
            {0x00034121, "BCHUNK_TRACKROUTE_MANAGER"},
            {0x00034122, "BCHUNK_TRACKROUTE_SIGNPOSTS"},
            {0x00034123, "BCHUNK_TRACKROUTE_TRAFFIC_INTERSECTIONS"},
            {0x00034124, "BCHUNK_TRACKROUTE_CROSS_TRAFFIC_EMITTERS"},
            {0x00034130, "BCHUNK_TOPOLOGYTREE"},
            {0x00034131, "BCHUNK_TOPOLOGYTREE"},
            {0x00034132, "BCHUNK_TOPOLOGYTREE"},
            {0x00034133, "BCHUNK_TOPOLOGYTREE"},
            {0x00034134, "BCHUNK_TOPOLOGYTREE"},
            {0x0003b300, "BCHUNK_WORLDOBJECTS"},
            {0x00034a09, "BCHUNK_PERFUPGRADELEVELINFOCHUNK"},
            {0x00034a0a, "BCHUNK_PERFUPGRADEPACKAGECHUNK"},
            {0x00030240, "BCHUNK_WIDEDECALS"},
            {0x00034a03, "BCHUNK_RANKINGLADDERS"},
            {0x00039010, "BCHUNK_SUBTITLES"},
            {0x00034035, "BCHUNK_NISSCENEDATA"},
            {0x80037020, "BCHUNK_ANIMSCENEDATA"},
            {0x0003b811, "BCHUNK_EVENTSEQUENCE"},
            {0x00034191, "Unknown TrackStreamer"},
            {0x80134001, "MapStream File Info"},
            {0x80134010, "MapStream Part"},
            {0x00000000, "Null Chunk"}
    };

    class JDLZ
    {
#define HEADER_SIZE 16
    public:
        static std::vector<BYTE> decompress(std::vector<BYTE> input);
    };

    class BitConverter
    {
    public:
        static int16_t ToInt16(const BYTE bytes[], int offset);

        static uint16_t ToUInt16(const BYTE bytes[], int offset);

        static int32_t ToInt32(const BYTE bytes[], int offset);

        static uint32_t ToUInt32(const BYTE bytes[], int offset);

        static uint64_t ToUInt64(const BYTE bytes[], int offset);

        static std::vector<BYTE> GetBytes(int value);

        static std::vector<BYTE> GetBytes(long value);
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

    template<typename N>
    N PaddingAlign(N num, int alignTo)
    {
        if (num % alignTo == 0)
            return 0;
        return alignTo - (num % alignTo);
    }

    template <typename d=void>
    long AlignFS(std::ifstream &ifstream, long bytes)
    {
        long b2skip = PaddingAlign(ifstream.tellg(), bytes);
        ifstream.ignore(b2skip);
        return b2skip;
    }

    BYTE readByte(std::ifstream &stream);

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
