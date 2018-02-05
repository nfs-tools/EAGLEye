#include "eaglutils.h"

namespace EAGLEye
{
    BYTE readByte(std::ifstream &stream)
    {
        return (BYTE) stream.get();
    }

    void dumpBytes(std::ifstream &stream, size_t bytes)
    {
        auto curPos = stream.tellg();

        unsigned char buf[bytes];

        stream.read((char *) &buf[0], sizeof(buf));
        hexdump(stdout, buf, sizeof(buf));
        stream.seekg(curPos);
    }

    long getFileLength(std::ifstream &stream)
    {
        stream.seekg(0, stream.end);

        long len = (long) stream.tellg();
        stream.seekg(0, stream.beg);
//        //TODO: this isn't supposed to work in binary, although it does. change it anyway.
//        fseek(pFile, 0, SEEK_END);
//        long len = ftell(pFile);
//        rewind(pFile);

        return len;
    }
}