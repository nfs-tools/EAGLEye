#ifndef EAGLEYE_CARBON_H
#define EAGLEYE_CARBON_H

#include "../chunkbase.h"

#include <boost/smart_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/filesystem.hpp>

namespace EAGLEye
{
    namespace Carbon
    {
        /**
         * BCHUNK_ELIGHTS structure (Global*.BUN)
         */
        struct MaterialDefinition
        {
            int32_t unknownSection[3]; // changes in memory
            int32_t hash;
            int32_t unk1;
            char name[0x40];
            int32_t diffuseClamp;
            int32_t diffuseFlakes;
            int32_t unk2;
            int32_t diffuseVinylScale;
            int32_t addRed, addCyan, addMagenta, addYellow;
            int32_t addRed2, addCyan2, addMagenta2, addYellow2;
            int32_t unknownSection2[2];
            int32_t whiteShine, whiteShine2;
            int32_t whiteShine3;
            int32_t cyan, cyan2;
            int32_t magenta;
            int32_t yellowMultiplier;
            int32_t weirdUnknown; // adds weird black FX at high values
            int32_t otherCyan, otherMagenta, otherYellow;
            int32_t unk4;
            int32_t envMap; // reflection or specularity
            int32_t unk5;
            int32_t weirdOther;
            int32_t weirdCyan;
            int32_t weirdMagenta;
            int32_t weirdYellow;
            int32_t specularReflection;
            int32_t redReflection;
            int32_t yellowReflection;
            int32_t magentaReflection;
            int32_t vinylLuminanceMinScale;
            int32_t vinylLuminanceMaxScale;
        };

        struct PACK GeometryFileInfo_s
        {
            uint32_t blnk1;
            uint32_t blnk2;
            uint32_t tTagA;
            uint32_t tTagB;
            char path[56];
            char section[4];
            uint32_t unknownData[7];
            uint32_t unkVarA;
            uint32_t unkVarB;
            uint32_t unkVarC;
            uint32_t unkData2[7];
        };

        struct PACK tFace
        {
            uint16_t vA;
            uint16_t vB;
            uint16_t vC;
        };

        struct PACK tVertex
        {
            float x, y, z;
            float nx, ny, nz;
            DWORD color;
            float u, v;
        };

        enum eBinChunkID
        {
            ID_FILE_HEADER = 0x134000,
            ID_DATA_HEADER = 0x134001,
            ID_BIN_ARCHIVE = 0x134002,
            ID_HASH_TABLE = 0x134003,//objects hash withing this bin
            ID_OBJECTS_LAYOUT = 0x134004,
            ID_BLANK = 0x134008,
            ID_ALIGN = 0x000000,
            ID_OBJECT = 0x134010,
            ID_OBJECT_HEADER = 0x134011,
            ID_TEXTURE_USAGE = 0x134012,
            ID_UNKNOWN2 = 0x134013,//?
            ID_UNKNOWN3 = 0x134F01,//?
            ID_TEXTURE_APPLY = 0x134015,
            ID_MOUNT_POINTS = 0x13401A,
            ID_MESH_HEADER = 0x134100,
            ID_MESH_DESCRIPTOR = 0x134900,
            ID_VERTICES = 0x134B01,
            ID_MAT_ASSIGN = 0x134B02,//?
            ID_UC_MAT_ASSIGN = 0x134902,
            ID_UC_VERTICES = 0x134901,
            ID_UC_TRIANGLES = 0x134903,
            ID_TRIANGLES = 0x134B03,
            ID_MATERIAL_NAME = 0x134C02,
            ID_UNKNOWN6 = 0x134017,// zero pad?
            ID_UNKNOWN7 = 0x134018,//?
            ID_UNKNOWN8 = 0x134019,//?

            ID_TPK_FILE = 0x310001,
            ID_TPK_TEXTURE_HASHES = 0x310002,
            ID_TPK_TEXTURE_NAMES = 0x310004,
            ID_TEX_BINARYDATA = 0x320002,

            ID_FORCE_DWORD = 0x80FFFFFF
        };

        struct CarbonBINChunk
        {
            eBinChunkID id;
            BYTE padding;
            const char *m_pszType;
        };

        struct PACK LanguageFileHeader_s
        {
            unsigned int numberOfStrings; // same across each individual language, of course
            unsigned int unknown1; // usually 0x00001814 in MW05 (14 18 00 00)
            unsigned int unknown2;
            char name[16];
        };

        void HandleFile(boost::filesystem::path &path, std::ifstream &ifstream, EAGLEye::FileType fileType);

        void HandleCarFile(boost::filesystem::path &path, std::ifstream &ifstream);

        void HandleLocationBaseFile(boost::filesystem::path &path, std::ifstream &ifstream);

        void HandleRegularFile(boost::filesystem::path &path, std::ifstream &ifstream);

        void HandleCompressedFile(boost::filesystem::path &path, std::ifstream &ifstream);
    }
}

#endif //EAGLEYE_CARBON_H
