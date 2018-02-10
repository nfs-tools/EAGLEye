#include "CarbonFontContainer.h"
#include <boost/algorithm/clamp.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        CarbonFontContainer::CarbonFontContainer(std::ifstream &stream, uint32_t containerSize) :
                Container(stream),
                m_containerSize(containerSize)
        {
        }

        std::shared_ptr<CarbonFont> CarbonFontContainer::Get()
        {
            CarbonFont font{};

            size_t bytesRead = 0;

            dumpBytes(m_stream, 256);

            m_stream.ignore(m_containerSize - bytesRead);

            return std::make_shared<CarbonFont>(font);
        }

        size_t CarbonFontContainer::ReadChunks(uint32_t totalSize)
        {
            return 0;
        }
    }
}