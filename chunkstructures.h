#ifndef EAGLEYE_STRUCTURES_H
#define EAGLEYE_STRUCTURES_H

#include "eaglutils.h"

namespace EAGLEye
{
    /**
     * oh heck it seems like these are embedded ELF files please help
     */
    struct AnimationsStruct_s
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
        BYTE postPad[12];
    };

    struct TrackStreamerSection_s
    {
        char id[4];
        BYTE data[88];
    };

    struct VisibleSectionStruct_s
    {
        BYTE pad[0x08];
        char name[40];
    };
}

#endif //EAGLEYE_STRUCTURES_H
