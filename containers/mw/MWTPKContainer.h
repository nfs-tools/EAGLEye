#ifndef EAGLEYE_MWTPKCONTAINER_H
#define EAGLEYE_MWTPKCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../data/tpk/TexturePack.h"

namespace EAGLEye
{
    namespace Containers
    {
        using TPK = EAGLEye::Data::TexturePack;

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

        /**
         * Chunk IDs relating to MW's TPK format
         */
        enum MWTPKChunks
        {
            MW_TPK = 0xb3310000,
            MW_TPK_TEXTURE_DATA_ROOT = 0xb3320000,
            MW_TPK_PART_INFO = 0x33310001,
            MW_TPK_PART_HASHES = 0x33310002,
            MW_TPK_PART_DYNAMIC_DATA = 0x33310003,
            MW_TPK_PART_HEADERS = 0x33310004,
            MW_TPK_PART_DXT_HEADERS = 0x3331005,
        };

        class MWTPKContainer : public Container<std::shared_ptr<TPK>>
        {
        public:
            MWTPKContainer(std::ifstream &stream, uint32_t containerSize);

            std::shared_ptr<TPK> Get() override;

        private:
            uint32_t m_containerSize;
            TPK *m_tpk;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}

#endif //EAGLEYE_MWTPKCONTAINER_H
