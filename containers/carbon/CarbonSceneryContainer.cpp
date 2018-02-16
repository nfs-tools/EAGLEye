#include "CarbonSceneryContainer.h"

#include <boost/algorithm/clamp.hpp>

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

                switch (id)
                {
                    case BCHUNK_SCENERY_SECTION_NAMES:
                    {
                        uint32_t entries = size / 72;

                        m_scenery.resize(entries);

                        for (int j = 0; j < entries; j++)
                        {
                            SceneryStruct sceneryStruct{};

                            bytesRead += readGeneric(m_stream, sceneryStruct);
                        }

                        break;
                    }
                    default:
                        break;
                }


                assert(bytesRead <= size);
//                dumpBytes(m_stream, boost::algorithm::clamp(size - bytesRead, 0, 512));

                totalBytesRead += bytesRead;
                m_stream.ignore(size - bytesRead);
                totalBytesRead += size - bytesRead;
            }

            return totalBytesRead;
        }
    }
}