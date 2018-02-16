#include "CarbonTriggerPackContainer.h"
#include <boost/algorithm/clamp.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        CarbonTriggerPackContainer::CarbonTriggerPackContainer(std::ifstream &stream, uint32_t containerSize)
            : Container(stream),
              m_containerSize(containerSize)
        {
        }

        void CarbonTriggerPackContainer::Get()
        {
            this->ReadChunks(m_containerSize);
        }

        size_t CarbonTriggerPackContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

                size_t bytesRead = 0;

//                printf("    TriggerPack Chunk #%02d: 0x%08x [%d bytes, @ 0x%08lx]\n", i + 1, id, size, (unsigned long) m_stream.tellg());

                assert(bytesRead <= size);

                totalBytesRead += size - bytesRead;
                m_stream.ignore(size - bytesRead);
                totalBytesRead += bytesRead;
            }

            return totalBytesRead;
        }
    }
}