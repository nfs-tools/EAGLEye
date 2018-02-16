#ifndef EAGLEYE_TEXTUREPACK_H
#define EAGLEYE_TEXTUREPACK_H

#include "../../eaglutils.h"

namespace EAGLEye
{
    namespace Data
    {
        class AnimatedTextureEntity
        {
        public:
            AnimatedTextureEntity()
            = default;

            ~AnimatedTextureEntity()
            = default;

            std::string name;
            int numFrames;
            int fps;
            std::vector<int32_t> frameHashes;
        };

        struct TextureInfo
        {
            int hash;
            int type;
            unsigned int dataOffset;
            unsigned int dataSize;
            unsigned int width;
            unsigned int height;
            int mipMap;
            std::string name;
            unsigned int ddsType;
        };

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
            std::vector<std::shared_ptr<AnimatedTextureEntity>> animations;
            std::map<int, std::shared_ptr<TextureInfo>> textures;
        };
    }
}

#endif //EAGLEYE_TEXTUREPACK_H
