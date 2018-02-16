#ifndef EAGLEYE_CARBONSOLIDLISTCONTAINER_H
#define EAGLEYE_CARBONSOLIDLISTCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../eagltypes.h"

namespace EAGLEye
{
    namespace Containers
    {
        struct PACK GeometryFileInfo
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

        struct PACK GeometryObjectHeader
        {
            BYTE zero[16]; // 16
            BYTE pad[4]; // +4 = 20
            long numTris; // +4 = 24
            BYTE pad2[8]; // +4 = 32
            Point3D minPoint; // +16 = 48
            Point3D maxPoint; // +16 = 64
            Matrix matrix; // +64 = 128
            DWORD unk3[8]; // +32 = 160
            std::string name;
        };

        enum CarbonSolidListChunkID
        {
            BCHUNK_CARBON_OBJECT = 0x80134010,
            BCHUNK_CARBON_OBJECT_HEADER = 0x00134011,
            BCHUNK_CARBON_OBJECT_TEXTURE_USAGE = 0x00134012,
            BCHUNK_CARBON_MESH_HEADER = 0x80134100,
            BCHUNK_CARBON_MESH_DESCRIPTOR = 0x00134900,
            BCHUNK_CARBON_MESH_VERTICES = 0x134b01,
        };

        struct CarbonSolidBINChunk
        {
            CarbonSolidListChunkID id;
            BYTE padding;
            const char *m_pszType;
        };

#pragma pack(push,2)
        struct tVertex
        {
            float   x,y,z;
            float   nx,ny,nz;
            DWORD   color;
            float   u,v;
        };
#pragma pack(pop)

        class CarbonSolidListContainer : public Container<void>
        {
        public:
            CarbonSolidListContainer(std::ifstream &stream, uint32_t containerSize);

            void Get() override;

        private:
            uint32_t m_containerSize;

            size_t ReadChunks(uint32_t totalSize) override;
        protected:
            static CarbonSolidBINChunk g_chunks[];
        };
    }
}


#endif //EAGLEYE_CARBONSOLIDLISTCONTAINER_H
