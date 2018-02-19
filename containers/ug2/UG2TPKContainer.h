#ifndef EAGLEYE_UG2TPKCONTAINER_H
#define EAGLEYE_UG2TPKCONTAINER_H

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
        * Chunk IDs relating to UG2's TPK format
        */
        enum UG2TPKChunks
        {
            UG2_TPK = 0xb3310000,
            UG2_TPK_PART_INFO = 0x33310001,
            UG2_TPK_PART_HASHES = 0x33310002,
            UG2_TPK_PART_DYNAMIC_DATA = 0x33310003,
            UG2_TPK_PART_HEADERS = 0x33310004,
            UG2_TPK_PART_DXT_HEADERS = 0x33310005,

            UG2_TPK_TEXTURE_DATA_ROOT = 0xb3320000,
            UG2_TPK_TEXTURE_DATA_HASH = 0x33320001,
            UG2_TPK_TEXTURE_DATA_CONTAINER = 0x33320002,
            UG2_TPK_TEXTURE_DATA_CONTAINER_XENON = 0x33320300,
        };

        class UG2TPKContainer : public Container<std::shared_ptr<TPK>>
        {
        public:
            UG2TPKContainer(std::ifstream &stream, uint32_t containerSize);

            std::shared_ptr<TPK> Get() override;

        private:
            uint32_t m_containerSize;
            TPK *m_tpk;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_UG2TPKCONTAINER_H
