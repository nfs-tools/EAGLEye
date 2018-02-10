#include "CarbonChunkyContainer.h"
#include "CarbonTPKContainer.h"
#include "CarbonLanguageContainer.h"
#include "CarbonFontContainer.h"
#include "CarbonSectionsContainer.h"
#include "CarbonSceneryContainer.h"

#include "../../chunk/ChunkIO.h"
#include "../../eaglutils.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        void CarbonChunkyContainer::Get()
        {
            long fileLength = getFileLength(this->m_stream);
            printf("Here we are! File length: %lu\n", fileLength);

            this->ReadChunks(static_cast<uint32_t>(fileLength));

//            this->ExtractToFolder(boost::filesystem::absolute("dump"));
//            this->CombineFromFiles(boost::filesystem::absolute("dump"), m_path.filename(),
//                                   boost::filesystem::path(m_path.stem().string() + std::string("-mod.BUN")));

//            EAGLEye::Chunks::ExtractChunks("data/carbon/TRACKS/L5RA.BUN", "data/carbon/TRACKS/L5RA-DUMP");
        }

        size_t CarbonChunkyContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                readGeneric(m_stream, id);
                readGeneric(m_stream, size);

                printf("Chunk #%02d: 0x%08x | ", i + 1, id);

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
                    case bChunkID::BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS:
                    {
                        auto *tpkContainer = new CarbonTPKContainer(m_stream, size);
                        auto tpk = tpkContainer->Get();

                        printf("Texture Pack: %s [%s | 0x%08x] - %zu texture(s)\n", tpk->name.c_str(),
                               tpk->tpkPath.c_str(), tpk->hash, tpk->hashTable.size());

                        break;
                    }
                    case bChunkID::BCHUNK_LANGUAGE:
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
                    case bChunkID::BCHUNK_FENG_FONT:
                    {
                        auto *fontContainer = new CarbonFontContainer(m_stream, size);
                        auto font = fontContainer->Get();

                        break;
                    }
                    case bChunkID::BCHUNK_TRACKSTREAMER_SECTIONS:
                    {
                        auto *sectionsContainer = new CarbonSectionsContainer(m_stream, size);
                        sectionsContainer->Get();

                        break;
                    }
                    case bChunkID::BCHUNK_SPEED_SCENERY_SECTION:
                    {
                        auto *sceneryContainer = new CarbonSceneryContainer(m_stream, size);
                        sceneryContainer->Get();
                        break;
                    }
                    case bChunkID::BCHUNK_PARAMETERMAPS:
                    {
                        dumpBytes(m_stream, size);
                    }
                    default:
                        printf("    * Passing unhandled chunk\n");
                        m_stream.ignore(size);
                        break;
                }
            }

            return 0;
        }
    }
}