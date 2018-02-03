#ifndef EAGLEYE_STRUCTURES_H
#define EAGLEYE_STRUCTURES_H

#include "eaglutils.h"

namespace EAGLEye
{
    /**
     * oh heck it seems like these are embedded ELF files please help
     */
    //
    struct PACK AnimationsStruct_s
    {
        BYTE pad[8];

        /**
         * ELF header
         */
        char eMagic[4];
        BYTE eFormat; // 1 for x86, 2 for x64, 3 for BOI WHAT
        BYTE eEndian; // 1 = LE, 2 = BE
        BYTE eVersion;
        BYTE eAbi;
        BYTE eAbiVersion;
        BYTE ePad[7];
        short eType;
        short eMachineType;
        int eVersionSecondary;
        int eEntryPoint;
        int ePHOffset;
        int eSHOffset;
        int eFlags;
        int eEhSize;
        int ePhentSize;
        int ePhnum;
        int eShentSize;
        int eShnum;
        int eShstrndx;

        /**
         * Back to animations
         */
        BYTE postPad[14];
    };

    struct PACK TrackStreamerSection_s
    {
        char id[4];
        unsigned int Unk1;
        unsigned int StreamChunkNumber;
        unsigned int Unk2;
        unsigned int MasterStreamChunkNumber;
        unsigned int MasterStreamChunkOffset;
        unsigned int Size1, Size2, Size3;
        float X, Y, Z;
        unsigned int StreamChunkHash;
        BYTE data[40];
    };

    struct PACK VisibleSectionStruct_s
    {
        BYTE pad[0x08];
        char name[40];
    };

    struct PACK FNGHeader_s
    {
        char id[3]; // 'FEn'
    };

    struct PACK LanguageEntry_s
    {
        unsigned int itemId;
        unsigned int unknown1;
    };

    struct PACK GEO_FACE
    {
        uint16_t vA;
        uint16_t vB;
        uint16_t vC;
    };

    struct PACK GEO_VERTEX
    {
        vec3 v_pos;
        uint32_t v_unk;
        vec2 v_uv;
    };
}

#endif //EAGLEYE_STRUCTURES_H
