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

        std::shared_ptr<EAGLEye::TrackPathChunk> ParseTrackPathChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);
        std::shared_ptr<EAGLEye::GeometryChunk> ParseGeometryChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);
        std::shared_ptr<EAGLEye::EAGLAnimationsChunk> ParseAnimationsChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);
        std::shared_ptr<EAGLEye::TrackStreamerSectionsChunk> ParseTrackStreamerSectionsChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);
        std::shared_ptr<EAGLEye::VisibleSectionChunk> ParseVisibleSectionChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);

        void HandleFile(boost::filesystem::path& path, std::ifstream& ifstream, EAGLEye::FileType fileType);
        void HandleLocationBaseFile(boost::filesystem::path &path, std::ifstream &ifstream);
        void HandleRegularFile(boost::filesystem::path &path, std::ifstream &ifstream);
        void HandleCompressedFile(boost::filesystem::path &path, std::ifstream &ifstream);
    }
}

#endif //EAGLEYE_MW_H
