#include "eaglutils.h"

namespace EAGLEye
{
    void dumpBytes(std::ifstream &stream, size_t bytes)
    {
        auto curPos = stream.tellg();

        unsigned char buf[bytes];

        stream.read((char *) &buf[0], sizeof(buf));
        hexdump(stdout, buf, sizeof(buf));
        stream.seekg(curPos);
    }
}