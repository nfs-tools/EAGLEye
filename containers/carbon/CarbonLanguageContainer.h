#ifndef EAGLEYE_CARBONLANGUAGECONTAINER_H
#define EAGLEYE_CARBONLANGUAGECONTAINER_H
#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../data/tpk/TexturePack.h"
#include "../../data/resources/LanguageDefinition.h"

namespace EAGLEye
{
    namespace Containers
    {
        using Language = EAGLEye::Data::LanguageDefinition;

        struct PACK LanguageFileHeader
        {
            unsigned int numberOfStrings; // same across each individual language, of course
            unsigned int unknown1; // usually 0x00001814 in MW05 (14 18 00 00)
            unsigned int unknown2;
            char name[16];
        };

        class CarbonLanguageContainer : public Container<std::shared_ptr<Language>>
        {
        public:
            CarbonLanguageContainer(std::ifstream& stream, uint32_t containerSize);

            std::shared_ptr<Language> Get() override;
        private:
            uint32_t m_containerSize;
            Language* m_language;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_CARBONLANGUAGECONTAINER_H
