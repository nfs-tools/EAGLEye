#include "PSSolidListContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        PSSolidListContainer::PSSolidListContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {
        }

        void PSSolidListContainer::Get()
        {
            this->ReadChunks(m_containerSize);
        }

        size_t PSSolidListContainer::ReadChunks(uint32_t totalSize)
        {
            return ReadChunks(totalSize, 0);
        }

        size_t PSSolidListContainer::ReadChunks(uint32_t totalSize, int depth)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            std::string padStr = "    ";

            for (int i = 0; i < depth; i++)
                padStr += "    ";

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

                printf("%sChunk #%02d: 0x%08x [%d bytes @ 0x%08lx]", padStr.c_str(), i + 1, id, size,
                       (unsigned long) m_stream.tellg());
                printf(" [%s]\n", BIN_CONTAINER(id) ? "Container" : "Container section");

                size_t bytesRead = 0;
                assert(bytesRead <= size);

                totalBytesRead += size - bytesRead;
                m_stream.ignore(size - bytesRead);
                totalBytesRead += bytesRead;
            }

            return totalBytesRead;
        }
    }
}