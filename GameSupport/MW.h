#ifndef EAGLEYE_MW_H
#define EAGLEYE_MW_H

#include "../chunkbase.h"

#include <boost/smart_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/filesystem.hpp>

namespace EAGLEye
{
    namespace MW
    {
        struct PACK tCarVertex
        {
            float x, y, z;
            float nx, ny, nz;
            DWORD color;
            float u, v;

            Point3D toPoint()
            {
                Point3D point{};
                point.x = x;
                point.y = y;
                point.z = z;
                return point;
            }
        };

        struct PACK tWorldVertex
        {
            float x, y, z;
            float nx, ny, nz;
            DWORD color;
            float u, v;

            Point3D toPoint()
            {
                Point3D point{};
                point.x = x;
                point.y = y;
                point.z = z;
                return point;
            }
        };

        struct PACK tFace
        {
            uint16_t vA;
            uint16_t vB;
            uint16_t vC;
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

            ID_FORCE_DWORD = 0x80FFFFFF
        };

        struct PACK LanguageFileHeader_s
        {
            unsigned int id; // should always be 0x00000010 (10 00 00 00)
            unsigned short numberOfStrings; // same across each individual language, of course
            unsigned int unknown1; // usually 0x00001814 in MW05 (14 18 00 00)
            unsigned int unknown2;
        };

        struct MWBINChunk
        {
            eBinChunkID id;
            BYTE padding;
            const char *m_pszType;
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

        struct PACK UnknownGeoBlock3_s
        {
            uint16_t unkValA_a;
            uint16_t unkValA_b;
            uint32_t unkValB;
        };

        struct PACK GeoEntityBlock_s
        {
            BYTE padding[8];
            uint32_t blnk1, blnk2, blnk3, unkA;
            uint16_t unkB_a, unkB_b;
            uint32_t unkC, unkD, unkE;
            vec3 vec1;
            uint32_t blnk4;
            vec3 vec2;
            uint32_t blnk5;
            float matrix64[16];
            uint32_t unkF, unkG;
            uint32_t ParA[2];
            uint32_t unkH;
            vec4 vec3;
            char name[32];
        };

        std::shared_ptr<EAGLEye::TrackPathChunk>
        ParseTrackPathChunk(std::ifstream &ifstream, uint32_t id, uint32_t size);

        std::shared_ptr<EAGLEye::GeometryChunk> ParseGeometryChunk(std::ifstream &ifstream, uint32_t id, uint32_t size);

        void ParseTexturePackChunk(std::ifstream &ifstream, uint32_t id, uint32_t size);

        std::shared_ptr<EAGLEye::EAGLAnimationsChunk>
        ParseAnimationsChunk(std::ifstream &ifstream, uint32_t id, uint32_t size);

        std::shared_ptr<EAGLEye::TrackStreamerSectionsChunk>
        ParseTrackStreamerSectionsChunk(std::ifstream &ifstream, uint32_t id, uint32_t size);

        std::shared_ptr<EAGLEye::VisibleSectionChunk>
        ParseVisibleSectionChunk(std::ifstream &ifstream, uint32_t id, uint32_t size);

        void GenerateStreamFile();

        void HandleFile(boost::filesystem::path &path, std::ifstream &ifstream, EAGLEye::FileType fileType);

        void HandleLocationBaseFile(boost::filesystem::path &path, std::ifstream &ifstream);

        void HandleRegularFile(boost::filesystem::path &path, std::ifstream &ifstream);

        void HandleCompressedFile(boost::filesystem::path &path, std::ifstream &ifstream);
    }
}

#endif //EAGLEYE_MW_H
