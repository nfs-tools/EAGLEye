#include "UG2SolidListContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        UG2SolidListContainer::UG2SolidListContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {
        }

        void UG2SolidListContainer::Get()
        {
            this->ReadChunks(m_containerSize);
        }

        size_t UG2SolidListContainer::ReadChunks(uint32_t totalSize)
        {
            m_stream.ignore(totalSize);

            return totalSize;
        }
    }
}