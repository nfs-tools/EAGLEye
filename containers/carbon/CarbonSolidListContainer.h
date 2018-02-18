#ifndef EAGLEYE_CARBONSOLIDLISTCONTAINER_H
#define EAGLEYE_CARBONSOLIDLISTCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../eagltypes.h"

namespace EAGLEye
{
    namespace Containers
    {
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
