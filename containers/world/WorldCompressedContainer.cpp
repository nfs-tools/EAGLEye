#include "WorldCompressedContainer.h"
#include <boost/algorithm/clamp.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        void WorldCompressedContainer::Get()
        {
            BYTE marker[4];
            readGenericArray(m_stream, marker, sizeof(marker));

            assert(marker[0] == 0x88 && marker[1] == 0x33 && marker[2] == 0x11 && marker[3] == 0x66);

            m_stream.ignore(4);

            uint32_t dataSize; // ???

            readGeneric(m_stream, dataSize);

            printf("Data Size: %d bytes\n", dataSize);

            long fileLength = getFileLength(this->m_stream);
            printf("Here we are! File length: %lu\n", fileLength);

            int blockIndex = 0;

            while (m_stream.tellg() < fileLength)
            {
                BYTE tmpBytes[4];
                readGenericArray(m_stream, tmpBytes, sizeof(tmpBytes));

                if (tmpBytes[0] == 0x22 && tmpBytes[1] == 0x11 && tmpBytes[2] == 0x44 && tmpBytes[3] == 0x55)
                {
                    printf("found compressed block\n");
                    int size, xsize;
                    readGeneric(m_stream, size);
                    readGeneric(m_stream, xsize);
                    printf("size = %d, xsize = %d\n", size, xsize);
                    m_stream.ignore(0xc);

                    auto pos = (long) m_stream.tellg();

                    char zipSign[4];
                    readGenericArray(m_stream, zipSign, 4);


                    if (zipSign[0] == 'J' && zipSign[1] == 'D' && zipSign[2] == 'L' && zipSign[3] == 'Z')
                    {
                        printf("JDLZ\n");

                        m_stream.ignore(4);

                        int jSize, jZSize;

                        readGeneric(m_stream, jSize);
                        readGeneric(m_stream, jZSize);

                        auto dumpPos = (long) m_stream.tellg();

                        m_stream.seekg(pos);

                        std::vector<BYTE> data;
                        data.resize(jZSize);

                        m_stream.read((char *) &data[0], jZSize);

                        std::vector<BYTE> uncompressed = JDLZ::decompress(data, jSize);
                        assert(uncompressed.size() == jSize);
                        hexdump(stdout, uncompressed.data(), 256);

                        printf("jSize = %d | jZSize = %d | dumpPos = %lu\n", jSize, jZSize, dumpPos);
                    }

                    m_stream.seekg((pos + xsize) - m_stream.tellg(), m_stream.cur);
//                    dumpBytes(m_stream, 12);
//                    m_stream.seekg(xsize, m_stream.cur);
                }
            }
        }

        size_t WorldCompressedContainer::ReadChunks(uint32_t totalSize)
        {
            return 0;
        }
    }
}