#ifndef EAGLEYE_EAGLTYPES_H
#define EAGLEYE_EAGLTYPES_H

#include "eagldefines.h"

namespace EAGLEye
{
    // Simple 3D point but with an additional coordinate
    struct PACK Point3D
    {
        float x, y, z, w;
    };

    union PACK Matrix
    {
        float m[16];
        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
    };

    struct PACK GeometryFileInfo
    {
        uint32_t blnk1;
        uint32_t blnk2;
        uint32_t tTagA;
        uint32_t tTagB;
        char path[56];
        char section[4];
        uint32_t unknownData[7];
        uint32_t unkVarA;
        uint32_t unkVarB;
        uint32_t unkVarC;
        uint32_t unkData2[7];
    };

    struct PACK GeometryObjectHeader
    {
        BYTE zero[16]; // 16
        BYTE pad[4]; // +4 = 20
        long numTris; // +4 = 24
        BYTE pad2[8]; // +4 = 32
        Point3D minPoint; // +16 = 48
        Point3D maxPoint; // +16 = 64
        Matrix matrix; // +64 = 128
        DWORD unk3[8]; // +32 = 160
        BYTE blnk[4];
        std::string name;
    };
}

#endif //EAGLEYE_EAGLTYPES_H
