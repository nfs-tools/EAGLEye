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
}

#endif //EAGLEYE_EAGLTYPES_H
