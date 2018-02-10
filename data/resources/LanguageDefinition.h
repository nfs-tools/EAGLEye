#ifndef EAGLEYE_LANGUAGEDEFINITION_H
#define EAGLEYE_LANGUAGEDEFINITION_H

#include "../../eaglutils.h"

namespace EAGLEye
{
    namespace Data
    {
        struct LanguageEntry
        {
            int hash1;
            int hash2;
            std::string text;
        };

        class LanguageDefinition
        {
        public:
            LanguageDefinition()
            = default;

            ~LanguageDefinition()
            = default;

            std::string name;
            int numberOfStrings;
            std::vector<LanguageEntry> entries;
        };
    }
}


#endif //EAGLEYE_LANGUAGEDEFINITION_H
