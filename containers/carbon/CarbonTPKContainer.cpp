#include "CarbonTPKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        CarbonTPKContainer::CarbonTPKContainer(std::ifstream &stream, uint32_t containerSize) :
                Container(stream),
                m_containerSize(containerSize)
        {
        }

        std::shared_ptr<TPK> CarbonTPKContainer::ReadData()
        {
            auto runTo = (long) m_stream.tellg() + m_containerSize;

            this->m_tpk = new EAGLEye::Data::TexturePack();

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                readGeneric(m_stream, id);
                readGeneric(m_stream, size);

                printf("    TPK Chunk #%02d: 0x%08x\n", i + 1, id);

                m_stream.ignore(size);
            }

            return std::make_shared<TPK>(*this->m_tpk);
        }
    }
}