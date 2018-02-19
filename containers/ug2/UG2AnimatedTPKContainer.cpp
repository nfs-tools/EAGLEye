#include "UG2AnimatedTPKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        UG2AnimatedTPKContainer::UG2AnimatedTPKContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {

        }

        std::shared_ptr<TPK> UG2AnimatedTPKContainer::Get()
        {
            this->m_tpk = new EAGLEye::Data::TexturePack();

            dumpBytes(m_stream, m_containerSize);
            this->ReadChunks(m_containerSize);

            return std::make_shared<TPK>(*m_tpk);
        }

        size_t UG2AnimatedTPKContainer::ReadChunks(uint32_t totalSize)
        {
            return ReadChunks(totalSize, 0);
        }

        size_t UG2AnimatedTPKContainer::ReadChunks(uint32_t totalSize, int depth)
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

                switch (id)
                {
                    case UG2_ANIMATED_TPK_ENTRIES:
                    {
                        for (int j = 0; j < size / sizeof(UG2AnimatedTexture); j++)
                        {
                            UG2AnimatedTexture animatedTexture{};
                            bytesRead += readGeneric(m_stream, animatedTexture);

                            printf("#%d: %s (0x%08x) [%d frames / %d fps]\n", j + 1, animatedTexture.name, animatedTexture.hash, animatedTexture.numFrames, animatedTexture.fps);

                            EAGLEye::Data::AnimatedTextureEntity animatedTextureEntity{};
                            animatedTextureEntity.name = std::string(animatedTexture.name);
                            animatedTextureEntity.numFrames = animatedTexture.numFrames;
                            animatedTextureEntity.fps = animatedTexture.fps;

                            m_tpk->animations.emplace_back(
                                    std::make_shared<EAGLEye::Data::AnimatedTextureEntity>(animatedTextureEntity));
                        }

                        break;
                    }
                    case UG2_ANIMATED_TPK_HASHES:
                    {
                        /**
                         * each entry = 4-byte hash + 0xC bytes of 0x00
                         */
                        for (int j = 0; j < size / 16; j++)
                        {
                            int hash;
                            bytesRead += readGeneric(m_stream, hash);
                            m_stream.ignore(0xC);
                            bytesRead += 0xC;

                            printf("Frame Hash #%d: 0x%08x\n", j + 1, hash);

                            m_tpk->animations[m_tpk->animations.size() - 1]->frameHashes.emplace_back(hash);
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