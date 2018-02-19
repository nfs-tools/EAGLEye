#include "WorldCompressedContainer.h"
#include <boost/algorithm/clamp.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        void WorldCompressedContainer::Get()
        {
            long fileLength = getFileLength(this->m_stream);
            printf("Here we are! File length: %lu\n", fileLength);

            m_stream.seekg(0x10);

            for (int i = 0; i < 1; i++)
            {
                int32_t sign;

                readGeneric(m_stream, sign);

                if (sign == 0x55441122)
                {
                    uint32_t size, xsize;
                    readGeneric(m_stream, size);
                    readGeneric(m_stream, xsize);

                    m_stream.ignore(0xc);

                    auto curPos = (long) m_stream.tellg();
                    char zipSign[4];

                    readGenericArray(m_stream, zipSign, 4);

                    if (zipSign[0] == 'J' && zipSign[1] == 'D' && zipSign[2] == 'L' && zipSign[3] == 'Z')
                    {
                        printf("JDLZ!\n");

                        int skipBytes = 0x10;

                        BYTE zipFlag = readByte(m_stream),
                                dummyB = readByte(m_stream);
                        m_stream.ignore(2);

                        int bSize, bzSize;

                        readGeneric(m_stream, bSize);
                        readGeneric(m_stream, bzSize);

                        printf("bSize = %d, bzSize = %d\n", bSize, bzSize);

                        m_stream.seekg(curPos);

                        std::vector<BYTE> compressed, uncompressed;
                        compressed.resize(bzSize);
                        uncompressed.resize(bSize);

                        m_stream.read((char *) &compressed[0], compressed.size());

                        uncompressed = JDLZ::decompress(compressed, bSize);

                        hexdump(stdout, uncompressed.data(), 512);

//                        curPos = (long) m_stream.tellg();
                    }

                    dumpBytes(m_stream, 256);
                }
            }

//            dumpBytes(m_stream, 512);
        }

        size_t WorldCompressedContainer::ReadChunks(uint32_t totalSize)
        {
            return 0;
        }
    }
}