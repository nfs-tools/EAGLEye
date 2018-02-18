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
