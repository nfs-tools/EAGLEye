#include "TexturePack.h"

void EAGLEye::Data::tDDSHeader::init(EAGLEye::Data::TextureInfo &info)
{
    m_dwMagic = 0x20534444; //"DDS "
    m_dwSize = 0x7C;
    m_dwFlags = 0x81007;//DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_LINEARSIZE;
    m_dwHeight = info.height;
    m_dwWidth = info.width;
    m_dwPitchOrLinearSize = info.dataSize;
    //pixel format:
    m_PixelFormat.dwSize = 0x20;
    m_PixelFormat.dwFlags = ((info.ddsType & 0xFFFF) == 0x5844) ? 4//DDPF_FOURCC
                                                                : 0;
    m_PixelFormat.dwFourCC = info.ddsType;
    if (0 == m_PixelFormat.dwFlags)
    {
        m_PixelFormat.dwRBitMask = 0xFF000000;
        m_PixelFormat.dwGBitMask = 0x00FF0000;
        m_PixelFormat.dwBBitMask = 0x0000FF00;
        m_PixelFormat.dwRGBAlphaBitMask = 0x000000FF;
    }
    //caps:
    m_ddsCaps.dwCaps1 = 0x1000;//DDSCAPS_TEXTURE;
}
