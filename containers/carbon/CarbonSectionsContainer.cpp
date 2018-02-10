#include "CarbonSectionsContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        CarbonSectionsContainer::CarbonSectionsContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {
        }

        Sections CarbonSectionsContainer::Get()
        {
            Sections sections{};

            size_t bytesRead = 0;
            auto runTo = (long) m_stream.tellg() + m_containerSize;

            int numOfSections = m_containerSize / sizeof(StreamInfoStruct);

            for (int i = 0; i < numOfSections; i++)
            {
                StreamInfoStruct streamInfo{};

                bytesRead += readGeneric(m_stream, streamInfo);

                printf("Group name:   %s\n", streamInfo.ModelGroupName);
                printf("Chunk number: %d\n", streamInfo.StreamChunkNumber);
            }

            assert(bytesRead <= m_containerSize);

            m_stream.ignore(m_containerSize - bytesRead);

            return sections;
        }

        size_t CarbonSectionsContainer::ReadChunks(uint32_t totalSize)
        {
            return 0;
        }
    }
}