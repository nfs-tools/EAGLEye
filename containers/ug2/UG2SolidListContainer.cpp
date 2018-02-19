#include "UG2SolidListContainer.h"
#include "../../eagltypes.h"
#include "../carbon/CarbonSolidListContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        UG2SolidBINChunk UG2SolidListContainer::g_chunks[] = {
                {BCHUNK_UG2_SOLID_OBJECT_HEADER,        0x10, "Object header"},
                {BCHUNK_UG2_SOLID_OBJECT_MESH_VERTICES, 0x10, "Vertices"},
        };

        UG2SolidListContainer::UG2SolidListContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {
        }

        std::shared_ptr<Catalog> UG2SolidListContainer::Get()
        {
            this->m_catalog = new EAGLEye::Data::SolidCatalog();
            this->ReadChunks(m_containerSize);

            return std::make_shared<Catalog>(*m_catalog);
        }

        size_t UG2SolidListContainer::ReadChunks(uint32_t totalSize)
        {
//            m_stream.ignore(totalSize);

            return ReadChunks(totalSize, 0);
        }

        size_t UG2SolidListContainer::ReadChunks(uint32_t totalSize, int depth)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            std::string padStr = "    ";

            for (int i = 0; i < depth; i++)
                padStr += "    ";

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

                printf("%sChunk #%02d: 0x%08x [%d bytes @ 0x%08lx]", padStr.c_str(), i + 1, id, size,
                       (unsigned long) m_stream.tellg());
                printf(" [%s]\n", BIN_CONTAINER(id) ? "Container" : "Container section");

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
                    case BCHUNK_UG2_SOLID_HEADER:
                    {
                        bytesRead += this->ReadChunks(size, 1);
                        break;
                    }
                    case BCHUNK_UG2_SOLID_FILE_INFO:
                    {
                        GeometryFileInfo geometryFileInfo{};
                        bytesRead += readGeneric(m_stream, geometryFileInfo);

                        this->m_catalog->path = std::string(geometryFileInfo.path);
                        this->m_catalog->section = std::string(geometryFileInfo.section);
                        printf("%s    Catalog: %s [%s]\n", padStr.c_str(), this->m_catalog->path.c_str(),
                               this->m_catalog->section.c_str());

                        break;
                    }
                    case BCHUNK_UG2_SOLID_HASH_TABLE:
                    {
                        this->m_catalog->hashTable.resize(size / 8);

                        printf("%s    Objects: %zu\n", padStr.c_str(), this->m_catalog->hashTable.size());

                        for (int j = 0; j < this->m_catalog->hashTable.size(); j++)
                        {
                            bytesRead += readGeneric(m_stream, this->m_catalog->hashTable[j]);

                            m_stream.ignore(4);
                            bytesRead += 4;

                            printf("%s    Object #%d: 0x%08x\n", padStr.c_str(), j + 1, this->m_catalog->hashTable[j]);
                        }

                        break;
                    }
                    case BCHUNK_UG2_SOLID_OBJECT: // Container chunk
                    {
                        bytesRead += this->ReadChunks(size, 1);
                        break;
                    }
                    case BCHUNK_UG2_SOLID_OBJECT_HEADER:
                    {
                        GeometryObjectHeader objectHeader{};
                        bytesRead += readGeneric(m_stream, objectHeader, 164);

                        size_t nameLen = size - 164 + 1;
                        char name[nameLen];

                        m_stream.read(name, nameLen - 1);
                        bytesRead += nameLen - 1;

                        objectHeader.name = std::string(name);
                        printf("%s    Object: %s\n", padStr.c_str(), name);

                        EAGLEye::Data::SolidItem solidItem{};
                        solidItem.name = objectHeader.name;

                        m_catalog->items.emplace_back(solidItem);

                        break;
                    }
                    case BCHUNK_UG2_SOLID_OBJECT_MESH_HEADER:
                    {
                        bytesRead += this->ReadChunks(size, 2);
                        break;
                    }
                    case BCHUNK_UG2_SOLID_OBJECT_MESH_VERTICES:
                    {
                        // Cars have different vertex formats... this is really a big hack. Why is this the case, though?
                        if (m_catalog->section == "DEFAULT" &&
                            m_catalog->path.find("\\CARS\\") != std::string::npos)
                        {
                            for (int j = 0; j < size / sizeof(tUGVertexV2); j++)
                            {
                                tUGVertexV2 vertex{};
                                bytesRead += readGeneric(m_stream, vertex);

                                printf("v %f %f %f\n", vertex.x, vertex.y, vertex.z);

                                EAGLEye::Data::MeshVertex meshVertex{};
                                meshVertex.x = vertex.x;
                                meshVertex.y = vertex.y;
                                meshVertex.z = vertex.z;
                                meshVertex.u = vertex.u;
                                meshVertex.v = vertex.v;

                                m_catalog->items[m_catalog->items.size() - 1].vertices.emplace_back(meshVertex);
                            }
                        } else
                        {
                            for (int j = 0; j < size / sizeof(tUGVertex); j++)
                            {
                                tUGVertex vertex{};
                                bytesRead += readGeneric(m_stream, vertex);

                                printf("v %f %f %f\n", vertex.x, vertex.y, vertex.z);

                                EAGLEye::Data::MeshVertex meshVertex{};
                                meshVertex.x = vertex.x;
                                meshVertex.y = vertex.y;
                                meshVertex.z = vertex.z;
                                meshVertex.u = vertex.u;
                                meshVertex.v = vertex.v;

                                m_catalog->items[m_catalog->items.size() - 1].vertices.emplace_back(meshVertex);
                            }
                        }

                        for (auto &vertex :  m_catalog->items[m_catalog->items.size() - 1].vertices)
                        {
                            printf("vt %f %f\n", vertex.u, vertex.v);
                        }

                        break;
                    }
                    case BCHUNK_UG2_SOLID_OBJECT_MESH_FACES:
                    {
                        for (int j = 0; j < size / sizeof(tUGFace); j++)
                        {
                            tUGFace face{};
                            bytesRead += readGeneric(m_stream, face);

                            printf("f %d %d %d\n", face.vA + 1, face.vB + 1, face.vC + 1);

                            EAGLEye::Data::MeshFace meshFace{};
                            meshFace.vA = face.vA;
                            meshFace.vB = face.vB;
                            meshFace.vC = face.vC;

                            m_catalog->items[m_catalog->items.size() - 1].faces.emplace_back(meshFace);
                        }
                        break;
                    }
                    case BCHUNK_UG2_SOLID_OBJECT_TEXTURE_USAGE:
                    {
                        /**
                         * Each entry is a 4-byte hash + 4-byte padding (0x00)
                         */

                        size_t numTextures = size / 8;
                        printf("%s    Number of Textures: %zu\n", padStr.c_str(), numTextures);

                        for (int j = 0; j < numTextures; j++)
                        {
                            int hash;
                            bytesRead += readGeneric(m_stream, hash);

                            m_stream.ignore(4);
                            bytesRead += 4;

                            printf("%s    Texture #%d: 0x%08x\n", padStr.c_str(), j + 1, hash);

                            m_catalog->items[m_catalog->items.size() - 1].textureIds.emplace_back(hash);
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