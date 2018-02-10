#ifndef EAGLEYE_TEXTUREPACK_H
#define EAGLEYE_TEXTUREPACK_H

#include "../../eaglutils.h"

namespace EAGLEye
{
    namespace Data
    {
        class TexturePack
        {
        public:
            TexturePack()
            = default;

            ~TexturePack()
            = default;

            std::string name;
            int32_t hash;
            std::string tpkPath;
            std::vector<int32_t> hashTable;
        };
    }
}

#endif //EAGLEYE_TEXTUREPACK_H
