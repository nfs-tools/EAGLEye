#include "CarbonChunkyContainer.h"
#include "CarbonTPKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        void CarbonChunkyContainer::ReadData()
        {
            long fileLength = getFileLength(this->m_stream);
            printf("Here we are! File length: %lu\n", fileLength);

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < fileLength; i++)
            {
                uint32_t id, size;
                readGeneric(m_stream, id);
                readGeneric(m_stream, size);

                printf("Chunk #%02d: 0x%08x | ", i + 1, id);

                auto chunkMapEntry = EAGLEye::chunkIdMap.find(id);

                if (chunkMapEntry == EAGLEye::chunkIdMap.end())
                {
                    printf("No name\n");
                } else
                {
                    printf("%s\n", chunkMapEntry->second.c_str());
                }

                switch (id)
                {
                    case bChunkID::BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS:
                    {
                        auto* tpkContainer = new CarbonTPKContainer(m_stream, size);
                        tpkContainer->ReadData();

                        break;
                    }
                    default:
                        printf("    * Passing unhandled chunk\n");
                        m_stream.ignore(size);
                        break;
                }
            }
        }
    }
}