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
            explicit TexturePack() = default;

            TexturePack(const TexturePack &c) = delete;

            TexturePack(const TexturePack &&c) = delete;

            TexturePack &operator=(const TexturePack &c) = delete;

            TexturePack &operator=(const TexturePack &&c) = delete;
        };
    }
}

#endif //EAGLEYE_TEXTUREPACK_H
