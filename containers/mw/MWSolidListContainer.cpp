#include "MWSolidListContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        MWSolidBINChunk MWSolidListContainer::g_chunks[] = {
                {BCHUNK_MW_SOLID_OBJECT_HEADER, 0x10, "Object header"},
                {BCHUNK_MW_SOLID_OBJECT_MESH_VERTICES, 0x10, "Vertices"},
        };

        MWSolidListContainer::MWSolidListContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {
        }

        void MWSolidListContainer::Get()
        {
            this->ReadChunks(m_containerSize);
        }

        size_t MWSolidListContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

//                printf("    Solid Chunk #%02d: 0x%08x [%d bytes @ 0x%08lx]\n", i + 1, id, size,
//                       (unsigned long) m_stream.tellg());

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

                switch (id)
                {
                    case BCHUNK_MW_SOLID_HEADER:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case BCHUNK_MW_SOLID_FILE_INFO:
                    {
                        GeometryFileInfo fileInfo{};
                        bytesRead += readGeneric(m_stream, fileInfo);

                        printf("    %s [%s]\n", fileInfo.path, fileInfo.section);

                        break;
                    }
                    case BCHUNK_MW_SOLID_HASH_TABLE:
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
                    case BCHUNK_MW_SOLID_OBJECT:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case BCHUNK_MW_SOLID_OBJECT_HEADER:
                    {
                        GeometryObjectHeader objectHeader{};
                        bytesRead += readGeneric(m_stream, objectHeader, 160);

                        size_t nameLen = size - 160 + 1;
                        char name[nameLen];

                        m_stream.read(name, nameLen - 1);
                        bytesRead += nameLen - 1;

                        objectHeader.name = std::string(name);
                        printf("        Object: %s\n", objectHeader.name.c_str());
                        break;
                    }
                    case BCHUNK_MW_SOLID_OBJECT_MESH_HEADER:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    /**
                     * Kinda broken... need to fix.
                     * All hail King 262212370559861843554378893038702297088.000000.
                     */
                    case BCHUNK_MW_SOLID_OBJECT_MESH_VERTICES:
                    {
                        for (int j = 0; j < size / sizeof(tMWVertex); j++)
                        {
                            tMWVertex vertex{};
                            bytesRead += readGeneric(m_stream, vertex);
                        }

                        break;
                    }
                    case BCHUNK_MW_SOLID_OBJECT_MESH_FACES:
                    {
                        for (int j = 0; j < size / sizeof(tMWFace); j++)
                        {
                            tMWFace face{};
                            bytesRead += readGeneric(m_stream, face);
                        }
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