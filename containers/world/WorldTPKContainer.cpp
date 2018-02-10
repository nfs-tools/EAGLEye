#include "WorldTPKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        WorldBINChunk g_chunks[] = {
                {BIN_ID(WORLD_TPK_TEXTURE_DATA), 0x80, "Texture Data"},
                {0x00000000,                     0x00, nullptr},
        };

        WorldTPKContainer::WorldTPKContainer(std::ifstream &stream, uint32_t containerSize) :
                Container(stream),
                m_containerSize(containerSize)
        {
        }

        std::shared_ptr<TPK> WorldTPKContainer::Get()
        {
            this->m_tpk = new EAGLEye::Data::TexturePack();
            this->ReadChunks(m_containerSize);

            return std::make_shared<TPK>(*m_tpk);
        }

        size_t WorldTPKContainer::ReadChunks(uint32_t totalSize)
        {
            return 0;
        }
    }
}