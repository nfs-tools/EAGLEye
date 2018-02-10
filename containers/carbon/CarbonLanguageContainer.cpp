#include "CarbonLanguageContainer.h"
#include <sstream>

namespace EAGLEye
{
    namespace Containers
    {
        CarbonLanguageContainer::CarbonLanguageContainer(std::ifstream &stream, uint32_t containerSize) :
                Container(stream),
                m_containerSize(containerSize)
        {
        }

        std::shared_ptr<Language> CarbonLanguageContainer::Get()
        {
            this->m_language = new EAGLEye::Data::LanguageDefinition();

            size_t bytesRead = 0;
            LanguageFileHeader languageFileHeader{};

            bytesRead += readGeneric(m_stream, languageFileHeader);

            m_language->name = std::string(languageFileHeader.name);
            m_language->numberOfStrings = languageFileHeader.numberOfStrings;

            {
                m_language->entries.resize(languageFileHeader.numberOfStrings);

                // Read hash table
                for (int i = 0; i < languageFileHeader.numberOfStrings; i++)
                {
                    bytesRead += readGeneric(m_stream, m_language->entries[i].hash1);
                    bytesRead += readGeneric(m_stream, m_language->entries[i].hash2);
                }

                // Read strings
                for (int i = 0; i < languageFileHeader.numberOfStrings; i++)
                {
                    std::stringstream textStream;

                    BYTE rb;
                    bytesRead += readGeneric(m_stream, rb);

                    while (rb != 0x00)
                    {
                        if (rb >= 32 && rb < 127)
                            textStream << rb;
                        else
                            textStream << ' ';
                        bytesRead += readGeneric(m_stream, rb);
                    }

                    m_language->entries[i].text = textStream.str();
                }
            }

            assert(bytesRead <= m_containerSize);

            m_stream.ignore(m_containerSize - bytesRead);

            return std::make_shared<Language>(*m_language);
        }

        size_t CarbonLanguageContainer::ReadChunks(uint32_t totalSize)
        {
            return 0;
        }
    }
}