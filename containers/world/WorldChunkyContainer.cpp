#include "WorldChunkyContainer.h"
#include "WorldTPKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        void WorldChunkyContainer::Get()
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
                    case BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS:
                    {
                        auto * tpkContainer = new WorldTPKContainer(m_stream, size);
                        tpkContainer->Get();

                        break;
                    }
                    default:
                        break;
                }

                m_stream.ignore(size);
            }
        }

        size_t WorldChunkyContainer::ReadChunks(uint32_t totalSize)
        {
            return 0;
        }
    }
}