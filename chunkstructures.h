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

    struct PACK LanguageFileHeader_s
    {
        unsigned int id; // should always be 0x00000010 (10 00 00 00)
        unsigned short numberOfStrings; // same across each individual language, of course
        unsigned int unknown1; // usually 0x00001814 in MW05 (14 18 00 00)
        unsigned int unknown2;
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
}

#endif //EAGLEYE_STRUCTURES_H
