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

        struct tDDSHeader
        {
            DWORD m_dwMagic;
            DWORD m_dwSize;
            DWORD m_dwFlags;
            DWORD m_dwHeight;
            DWORD m_dwWidth;
            DWORD m_dwPitchOrLinearSize;
            DWORD m_dwDepth;
            DWORD m_dwMipMapCount;
            DWORD m_dwReserved1[11];
            struct
            {
                DWORD dwSize;
                DWORD dwFlags;
                DWORD dwFourCC;
                DWORD dwRGBBitCount;
                DWORD dwRBitMask;
                DWORD dwGBitMask;
                DWORD dwBBitMask;
                DWORD dwRGBAlphaBitMask;
            } m_PixelFormat;
            struct
            {
                DWORD dwCaps1, dwCaps2, Reserved[2];
            } m_ddsCaps;
            DWORD m_dwReserved2;

            void init(EAGLEye::Data::TextureInfo &info);

            tDDSHeader()
            { memset(this, 0, sizeof(*this)); }
        };
    }
}

#endif //EAGLEYE_TEXTUREPACK_H
