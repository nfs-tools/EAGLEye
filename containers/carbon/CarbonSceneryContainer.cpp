#include "CarbonSceneryContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        CarbonSceneryContainer::CarbonSceneryContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream), m_containerSize(containerSize)
        {
        }

        Scenery CarbonSceneryContainer::Get()
        {
            this->ReadChunks(m_containerSize);

            return m_scenery;
        }

        size_t CarbonSceneryContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

                size_t bytesRead = 0;

                printf("    Scenery Chunk #%02d: 0x%08x\n", i + 1, id);

                switch (id)
                {
                    case BCHUNK_SCENERY_SECTION_NAMES:
                    {
                        uint32_t entries = size / 72;
                        printf("Entries: %d\n", entries);

                        m_scenery.resize(entries);

                        for (int j = 0; j < entries; j++)
                        {
                            SceneryStruct sceneryStruct{};

                            bytesRead += readGeneric(m_stream, sceneryStruct);

                            printf("#%d: %s\n", j + 1, sceneryStruct.name);
                        }

                        break;
                    }
                    default:
                        printf("    * Passing unhandled scenery chunk\n");
                        break;
                }


                assert(bytesRead <= size);
                dumpBytes(m_stream, size - bytesRead);

                totalBytesRead += bytesRead;
                m_stream.ignore(size - bytesRead);
                totalBytesRead += size - bytesRead;
            }

            return totalBytesRead;
        }
    }
}