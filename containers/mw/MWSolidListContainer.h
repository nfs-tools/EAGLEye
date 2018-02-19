#ifndef EAGLEYE_MWSOLIDLISTCONTAINER_H
#define EAGLEYE_MWSOLIDLISTCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../eagltypes.h"

namespace EAGLEye
{
    namespace Containers
    {
        enum MWSolidListChunkID
        {
            BCHUNK_MW_SOLID_HEADER = 0x80134001,
            BCHUNK_MW_SOLID_FILE_INFO = 0x00134002,
            BCHUNK_MW_SOLID_HASH_TABLE = 0x00134003,
            BCHUNK_MW_SOLID_OBJECT = 0x80134010,
            BCHUNK_MW_SOLID_OBJECT_HEADER = 0x00134011,

            BCHUNK_MW_SOLID_OBJECT_MESH_HEADER = 0x80134100,
            BCHUNK_MW_SOLID_OBJECT_MESH_VERTICES = 0x00134b01,
            BCHUNK_MW_SOLID_OBJECT_MAT_ASSIGN = 0x00134b02,
            BCHUNK_MW_SOLID_OBJECT_MESH_FACES = 0x00134b03,
            BCHUNK_MW_SOLID_OBJECT_TEXTURE_USAGE = 0x00134012
        };

        struct PACK tMWVertex
        {
            float x, y, z;
            float nx, ny, nz;
            DWORD color;
            float u, v;
        };

        struct PACK tMWFace
        {
            uint16_t vA, vB, vC;
        };

        struct MWSolidBINChunk
        {
            MWSolidListChunkID id;
            BYTE padding;
            const char *m_pszType;
        };

        class MWSolidListContainer: public Container<void>
        {
        public:
            MWSolidListContainer(std::ifstream &stream, uint32_t containerSize);

            void Get() override;

        private:
            uint32_t m_containerSize;

            size_t ReadChunks(uint32_t totalSize) override;
        protected:
            static MWSolidBINChunk g_chunks[];
        };
    }
}


#endif //EAGLEYE_MWSOLIDLISTCONTAINER_H
