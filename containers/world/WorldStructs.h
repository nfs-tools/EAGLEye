#ifndef EAGLEYE_WORLDSTRUCTS_H
#define EAGLEYE_WORLDSTRUCTS_H

#include "../../eaglenums.h"

namespace EAGLEye
{
    struct WorldBINChunk
    {
        int chunkID;
        int padding;
        const char* desc;
    };
}

#endif //EAGLEYE_WORLDSTRUCTS_H
