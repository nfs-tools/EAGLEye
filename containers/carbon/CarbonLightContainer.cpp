#include "CarbonLightContainer.h"
#include <boost/algorithm/clamp.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        CarbonLightBINChunk CarbonLightContainer::g_chunks[] = {
                {BCHUNK_CARBON_LIGHT_UNKNOWN, 0x10, "Light hashes?"},
                {BCHUNK_CARBON_LIGHT_ENTRIES, 0x10, "Light entries"},
        };

        CarbonLightContainer::CarbonLightContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {
        }

        void CarbonLightContainer::Get()
        {
            this->ReadChunks(m_containerSize);
        }

        size_t CarbonLightContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

                size_t bytesRead = 0;

                long nAlign = 0;
                long nChunkIndex;
                for (nChunkIndex = 0; g_chunks[nChunkIndex].m_pszType != nullptr; nChunkIndex++)
                    if (BIN_ID(id) == BIN_ID(g_chunks[nChunkIndex].id))
                        break;

                if (g_chunks[nChunkIndex].padding != 0)
                {
                    DWORD dw = 0x11111111;
                    while (0x11111111 == dw)
                    {
                        nAlign += 4;
                        readGeneric(m_stream, dw);
                    }

                    m_stream.seekg(-4, m_stream.cur);
                    nAlign -= 4;
                }

                totalBytesRead += nAlign;
                size -= nAlign;

                printf("    Light Chunk #%02d: 0x%08x [%d bytes @ 0x%08lx]\n", i + 1, id, size,
                       (unsigned long) m_stream.tellg());

                switch (id)
                {
                    case BCHUNK_CARBON_LIGHT_UNKNOWN:
                    {
                        break;
                    }
                    case BCHUNK_CARBON_LIGHT_ENTRIES:
                    {
                        // each entry = 96 bytes
                        // name is 34 bytes
                        uint32_t entries = size / 96;
                        printf("entries: %d\n", entries);

                        for (int j = 0; j < entries; j++)
                        {
                            BYTE entry[62];
                            char name[34];

                            bytesRead += readGenericArray(m_stream, entry, 62);
                            bytesRead += readGenericArray(m_stream, name, 34);

                            printf("%s\n", name);
                        }

                        break;
                    }
                    default:
                        break;
                }

                assert(bytesRead <= size);

                totalBytesRead += size - bytesRead;
                m_stream.ignore(size - bytesRead);
                totalBytesRead += bytesRead;
            }

            return totalBytesRead;
        }
    }
}