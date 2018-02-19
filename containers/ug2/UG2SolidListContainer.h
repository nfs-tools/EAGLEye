#ifndef EAGLEYE_UG2GEOMETRYCONTAINER_H
#define EAGLEYE_UG2GEOMETRYCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../data/world/SolidCatalog.h"

namespace EAGLEye
{
    namespace Containers
    {
        using Catalog = EAGLEye::Data::SolidCatalog;

        enum UG2SolidListChunkID
        {
            BCHUNK_UG2_SOLID_HEADER = 0x80134001,
            BCHUNK_UG2_SOLID_FILE_INFO = 0x00134002,
            BCHUNK_UG2_SOLID_HASH_TABLE = 0x00134003,
            BCHUNK_UG2_SOLID_OBJECT = 0x80134010,
            BCHUNK_UG2_SOLID_OBJECT_HEADER = 0x00134011,
            BCHUNK_UG2_SOLID_OBJECT_MESH_HEADER = 0x80134100,
            BCHUNK_UG2_SOLID_OBJECT_MESH_VERTICES = 0x00134b02,
        };

        struct PACK tUGVertex
        {
            float   x,y,z;
            DWORD   color;
            float   u,v;
        };

        struct PACK tUGFace
        {
            uint16_t vA, vB, vC;
        };

        struct UG2SolidBINChunk
        {
            UG2SolidListChunkID id;
            BYTE padding;
            const char *m_pszType;
        };

        class UG2SolidListContainer : public Container<std::shared_ptr<Catalog>>
        {
        public:
            UG2SolidListContainer(std::ifstream &stream, uint32_t containerSize);

            std::shared_ptr<Catalog> Get() override;

        private:
            uint32_t m_containerSize;

            size_t ReadChunks(uint32_t totalSize) override;
            size_t ReadChunks(uint32_t totalSize, int depth);

            Catalog* m_catalog;
        protected:
            static UG2SolidBINChunk g_chunks[];
        };
    }
}


#endif //EAGLEYE_UG2GEOMETRYCONTAINER_H
