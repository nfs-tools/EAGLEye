#include "UG2ChunkyContainer.h"
#include "UG2SolidListContainer.h"
#include "UG2TPKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        void UG2ChunkyContainer::Get()
        {
            long fileLength = getFileLength(this->m_stream);

            if (m_options.start != -1 && m_options.end != -1)
            {
                assert(m_options.end > m_options.start);

                fileLength = m_options.end - m_options.start;

                m_stream.seekg(m_options.start);
            }

            printf("Here we are! File length: %lu\n", fileLength);

            this->ReadChunks(static_cast<uint32_t>(fileLength));
        }

        size_t UG2ChunkyContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                readGeneric(m_stream, id);
                readGeneric(m_stream, size);

                printf("Chunk #%02d: 0x%08x [%d bytes, @ 0x%08lx] | ", i + 1, id, size,
                       (unsigned long) m_stream.tellg());

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
                    case BCHUNK_SPEED_ESOLID_LIST_CHUNKS:
                    {
                        auto* solidListContainer = new UG2SolidListContainer(m_stream, size);
                        solidListContainer->Get();
                        break;
                    }
                    case BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS:
                    {
                        auto* tpkContainer = new UG2TPKContainer(m_stream, size);
                        tpkContainer->Get();
                        break;
                    }
                    case BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS_ANIM:
                    {

                    }
                    default:
//                        printf("    * Passing unhandled chunk\n");
                        m_stream.ignore(size);
                        break;
                }
            }
            return 0;
        }
    }
}