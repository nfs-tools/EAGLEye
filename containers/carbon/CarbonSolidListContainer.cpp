#include "CarbonSolidListContainer.h"
#include <boost/algorithm/clamp.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        CarbonSolidBINChunk CarbonSolidListContainer::g_chunks[] = {
                {BCHUNK_CARBON_OBJECT_HEADER, 0x10, "Object header"},
                {BCHUNK_CARBON_MESH_VERTICES, 0x10, "Vertices"},
        };

        CarbonSolidListContainer::CarbonSolidListContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {
        }

        void CarbonSolidListContainer::Get()
        {
            this->ReadChunks(m_containerSize);
        }

        size_t CarbonSolidListContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

                size_t bytesRead = 0;

                long nAlign = 0;
                long nChunkIndex;
                for (nChunkIndex = 0; g_chunks[nChunkIndex].m_pszType != nullptr; nChunkIndex++)
                    if (BIN_ID(id) == BIN_ID(g_chunks[nChunkIndex].id))
                        break;

                if (g_chunks[nChunkIndex].padding != 0)
                {
                    DWORD dw = 0x11111111;
                    while (0x11111111 == dw)
                    {
                        nAlign += 4;
                        readGeneric(m_stream, dw);
                    }

                    m_stream.seekg(-4, m_stream.cur);
                    nAlign -= 4;
                }

                totalBytesRead += nAlign;
                size -= nAlign;

//                printf("    Solid Chunk #%02d: 0x%08x [%d bytes @ 0x%08lx]\n", i + 1, id, size,
//                       (unsigned long) m_stream.tellg());

                switch (id)
                {
                    case 0x80134001:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case 0x00134002: // File Info
                    {
                        GeometryFileInfo fileInfo{};
                        bytesRead += readGeneric(m_stream, fileInfo);

//                        printf("%s [%s]\n", fileInfo.path, fileInfo.section);

                        break;
                    }
                    case 0x00134003: // Hashes
                    {
                        int numEntries = size / 8; // 4-byte hash, 4-byte pad

                        for (int j = 0; j < numEntries; j++)
                        {
                            int hash;
                            bytesRead += readGeneric(m_stream, hash);
                            m_stream.ignore(4);
                            bytesRead += 4;

//                            printf("Hash #%d: 0x%08x\n", j + 1, hash);
                        }

                        break;
                    }
                    case BCHUNK_CARBON_OBJECT:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case BCHUNK_CARBON_OBJECT_HEADER:
                    {
                        GeometryObjectHeader objectHeader{};
                        bytesRead += readGeneric(m_stream, objectHeader, 160);

                        size_t nameLen = size - 160 + 1;
                        char name[nameLen];

                        m_stream.read(name, nameLen - 1);
                        bytesRead += nameLen - 1;

                        objectHeader.name = std::string(name);
                        printf("Object: %s\n", objectHeader.name.c_str());

                        break;
                    }
                    case BCHUNK_CARBON_OBJECT_TEXTURE_USAGE:
                    {
                        break;
                    }
                    case BCHUNK_CARBON_MESH_HEADER:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case BCHUNK_CARBON_MESH_DESCRIPTOR:
                    {
                        break;
                    }
                    default:
                        break;
                }

                assert(bytesRead <= size);

                totalBytesRead += size - bytesRead;
                m_stream.ignore(size - bytesRead);
                totalBytesRead += bytesRead;
            }

            return totalBytesRead;
        }
    }
}