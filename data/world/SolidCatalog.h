#ifndef EAGLEYE_SOLIDCATALOG_H
#define EAGLEYE_SOLIDCATALOG_H

#include "../../eaglutils.h"

namespace EAGLEye
{
    namespace Data
    {
        struct SolidItem
        {
            std::string name;
            int hash;
        };

        class SolidCatalog
        {
        public:
            SolidCatalog()
            = default;

            ~SolidCatalog()
            = default;
        };
    }
}


#endif //EAGLEYE_SOLIDCATALOG_H
