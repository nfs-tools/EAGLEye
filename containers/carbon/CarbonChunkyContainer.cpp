#include "CarbonChunkyContainer.h"
#include "CarbonTPKContainer.h"
#include "CarbonLanguageContainer.h"
#include "CarbonFontContainer.h"
#include "CarbonSectionsContainer.h"
#include "CarbonSceneryContainer.h"
#include "CarbonTriggerPackContainer.h"
#include "CarbonSolidListContainer.h"
#include "CarbonLightContainer.h"

#include "../../chunk/ChunkIO.h"
#include "../../eaglutils.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/clamp.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        void CarbonChunkyContainer::Get()
        {
            long fileLength = getFileLength(this->m_stream);
            printf("Here we are! File length: %lu\n", fileLength);

            this->m_animationManager = new CarbonAnimationManager;
            this->ReadChunks(static_cast<uint32_t>(fileLength));

//            this->ExtractToFolder(boost::filesystem::absolute("dump"));
//            this->CombineFromFiles(boost::filesystem::absolute("dump"), m_path.filename(),
//                                   boost::filesystem::path(m_path.stem().string() + std::string("-mod.BUN")));

//            EAGLEye::Chunks::ExtractChunks("data/carbon/TRACKS/L5RA.BUN", "data/carbon/TRACKS/L5RA-DUMP");
        }

        void padHelper(std::ifstream& stream)
        {
            while (readByte(stream) == 0x11)
            {}

            stream.seekg(-1, stream.cur);
        }

        size_t CarbonChunkyContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                readGeneric(m_stream, id);
                readGeneric(m_stream, size);

                printf("Chunk #%02d: 0x%08x [%d bytes, @ 0x%08lx] | ", i + 1, id, size,
                       (unsigned long) m_stream.tellg());

                auto chunkMapEntry = EAGLEye::chunkIdMap.find(id);

                if (chunkMapEntry == EAGLEye::chunkIdMap.end())
                {
                    printf("No name\n");
                } else
                {
                    printf("%s\n", chunkMapEntry->second.c_str());
                }

                switch (id)
                {
                    case BCHUNK_SPEED_ESOLID_LIST_CHUNKS:
                    {
                        auto* solidsContainer = new CarbonSolidListContainer(m_stream, size);
                        solidsContainer->Get();
                        break;
                    }
                    case BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS:
                    {
                        auto *tpkContainer = new CarbonTPKContainer(m_stream, size);
                        auto tpk = tpkContainer->Get();

                        printf("Texture Pack: %s [%s | 0x%08x] - %zu texture(s)\n", tpk->name.c_str(),
                               tpk->tpkPath.c_str(), tpk->hash, tpk->hashTable.size());

                        int textureIndex = 0;

                        for (auto &texturePair : tpk->textures)
                        {
                            auto &texture = texturePair.second;

                            printf("Texture #%d: %s (0x%08x, %d by %d)\n", textureIndex + 1, texture->name.c_str(),
                                   texture->hash, texture->width, texture->height);

                            textureIndex++;
                        }

                        textureIndex = 0;

                        for (auto &animation : tpk->animations)
                        {
                            printf("Animation #%d: %s (%d frames @ %d FPS)\n", textureIndex + 1,
                                   animation->name.c_str(), animation->numFrames, animation->fps);

                            for (int j = 0; j < animation->numFrames; j++)
                            {
                                printf("    Frame %d: 0x%08x (%s)\n", j + 1, animation->frameHashes[j],
                                       tpk->textures[animation->frameHashes[j]]->name.c_str());
                            }

                            textureIndex++;
                        }

                        break;
                    }
                    case BCHUNK_LANGUAGE:
                    {
                        auto *languageContainer = new CarbonLanguageContainer(m_stream, size);
                        auto language = languageContainer->Get();

                        printf("Language: %s [%d strings]\n", language->name.c_str(), language->numberOfStrings);

                        for (int j = 0; j < language->numberOfStrings; j++)
                        {
                            printf("    Entry #%d: %s\n", j + 1, language->entries[j].text.c_str());
                        }

                        break;
                    }
                    case BCHUNK_FENG_FONT:
                    {
                        auto *fontContainer = new CarbonFontContainer(m_stream, size);
                        auto font = fontContainer->Get();

                        break;
                    }
                    case BCHUNK_TRACKSTREAMER_SECTIONS:
                    {
                        auto *sectionsContainer = new CarbonSectionsContainer(m_stream, size);
                        sectionsContainer->Get();

                        break;
                    }
                    case BCHUNK_SPEED_SCENERY_SECTION:
                    {
                        auto *sceneryContainer = new CarbonSceneryContainer(m_stream, size);
                        sceneryContainer->Get();
                        break;
                    }
                    case BCHUNK_SPEED_EMTRIGGER_PACK:
                    {
                        auto *triggerPackContainer = new CarbonTriggerPackContainer(m_stream, size);
                        triggerPackContainer->Get();
                        break;
                    }
                    case BCHUNK_SPEED_BBGANIM_BLOCKHEADER:
                    {
                        AnimationBlockHeader header{};
                        readGeneric(m_stream, header);
                        this->m_animationManager->PushBlock(header);
                        break;
                    }
                    case BCHUNK_SPEED_BBGANIM_INSTANCE_TREE:
                    {
                        padHelper(m_stream);

                        AnimationBlockTree tree{};
                        readGeneric(m_stream, tree);
                        this->m_animationManager->PushTree(tree);
                        break;
                    }
                    case BCHUNK_SPEED_BBGANIM_INSTANCE_NODE:
                    {
                        AnimationBlockNode node{};
                        readGeneric(m_stream, node);
                        this->m_animationManager->PushNode(node);
                        break;
                    }
                    case BCHUNK_SPEED_ELIGHT_CHUNKS:
                    {
                        auto* lightContainer = new CarbonLightContainer(m_stream, size);
                        lightContainer->Get();
                        break;
                    }
                    default:
//                        printf("    * Passing unhandled chunk\n");
                        m_stream.ignore(size);
                        break;
                }
            }

            return 0;
        }
    }
}