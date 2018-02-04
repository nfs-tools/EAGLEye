#include "World.h"
#include <iostream>

namespace EAGLEye
{
    namespace World
    {
        WorldBINChunk g_chunks[] = {
                {ID_FILE_HEADER,     0x00, "File Header"},
                {ID_DATA_HEADER,     0x00, "Data Header"}, //includes hash table
                {ID_BIN_ARCHIVE,     0x00, "BIN archive"},
                {ID_HASH_TABLE,      0x00, "hash table?"},
                {ID_OBJECTS_LAYOUT,  0x00, "hash mapping?"},
                {ID_BLANK,           0x00, "blank?"},
                {ID_ALIGN,           0x00, "align block?"},
                {ID_OBJECT,          0x00, "Object"},
                {ID_OBJECT_HEADER,   0x10, "Object header?"},
                {ID_TEXTURE_USAGE,   0x00, "Texture Usage"},//material burst
                {ID_UNKNOWN2,        0x00, "unknown#2"},
                {ID_UNKNOWN3,        0x80, "unknown#2"},
                {ID_TEXTURE_APPLY,   0x00, "Texture apply modes"},
                {ID_MOUNT_POINTS,    0x10, "mount points"},
                {ID_MESH_HEADER,     0x00, "mesh header"},
                {ID_MESH_DESCRIPTOR, 0x10, "mesh descriptor"},
                {ID_VERTICES,        0x10, "vertices"},     //#15.10.2007 align changed from 0x80 to 0x10
                {ID_UC_VERTICES,     0x10, "UC vertices"},
                {ID_UC_MAT_ASSIGN,   0x80, "mat/sh assign UC"},
                {ID_MAT_ASSIGN,      0x10, "mat/sh assign?"},
                {ID_TRIANGLES,       0x10, "triangles"},
                {ID_UC_TRIANGLES,    0x10, "UC triangles"},
                {ID_MATERIAL_NAME,   0x00, "material name"},
                {ID_UNKNOWN6,        0x00, "unknown#6"},
                {ID_UNKNOWN7,        0x00, "unknown#7"}, // looks like 0x4003
                {ID_UNKNOWN8,        0x00, "unknown#8"}, // looks like 0x4004
                {ID_ALIGN,           0x00, nullptr},
        };

        // region Chunk Parsers

        size_t HandleGeometrySubChunkParts(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            auto runTo = ((long) ifstream.tellg()) + size;

            size_t bytesRead = 0;

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < runTo; i++)
            {
                uint32_t partId, partSize;
                size_t partBytesRead = 0;

                bytesRead += readGeneric(ifstream, partId);
                bytesRead += readGeneric(ifstream, partSize);

                printf("        Part: 0x%08x\n", partId);

                long nAlign = 0;
                long nChunkIndex;
                for (nChunkIndex = 0; g_chunks[nChunkIndex].m_pszType != nullptr; nChunkIndex++)
                    if (BIN_ID(partId) == BIN_ID(g_chunks[nChunkIndex].id))
                        break;

                if (g_chunks[nChunkIndex].padding != 0)
                {
                    DWORD dw = 0x11111111;
                    while (0x11111111 == dw)
                    {
                        nAlign += 4;
                        readGeneric(ifstream, dw);
                    }

                    ifstream.seekg(-4, ifstream.cur);
                    nAlign -= 4;
                }

                bytesRead += nAlign;
                partSize -= nAlign;

                switch (partId)
                {
                    // We can use BCHUNK_CARBON_SOLIDS_* because there's no reason not to
                    case bChunkID::BCHUNK_CARBON_SOLIDS_ARCHIVE:
                    {
                        GeometryFileInfo_s geometryFileInfo{};
                        partBytesRead += readGeneric(ifstream, geometryFileInfo);

                        printf("%s [%s]\n", geometryFileInfo.path, geometryFileInfo.section);

                        break;
                    }
                    case bChunkID::BCHUNK_CARBON_SOLIDS_HASH_TABLE:
                    {
                        size_t numEntries = partSize / 8;

                        printf("Entries: %zu\n", numEntries);

                        for (int j = 0; j < numEntries; j++)
                        {
                            int32_t hash;
                            partBytesRead += readGeneric(ifstream, hash);
                            ifstream.ignore(4);
                            partBytesRead += 4;

                            printf("Entry #%d: 0x%08x\n", j + 1, hash);
                        }

                        break;
                    }
                    case bChunkID::BCHUNK_CARBON_SOLIDS_TEXTURE_USAGE:
                    {
                        size_t numEntries = partSize / 8;

                        printf("Used Textures: %zu\n", numEntries);

                        for (int j = 0; j < numEntries; j++)
                        {
                            int32_t hash;
                            partBytesRead += readGeneric(ifstream, hash);
                            ifstream.ignore(4);
                            partBytesRead += 4;

                            printf("Texture #%d: 0x%08x\n", j + 1, hash);
                        }
                        break;
                    }
                    case bChunkID::BCHUNK_CARBON_SOLIDS_OBJECT_HEADER:
                    {
                        ifstream.ignore(12);
                        partBytesRead += 12; // 3 zeroes

                        uint32_t unk;
                        partBytesRead += readGeneric(ifstream, unk);

                        printf("unk1 = %d\n", unk);

                        uint32_t unk2;
                        partBytesRead += readGeneric(ifstream, unk2);

                        long numTris;
                        partBytesRead += readGeneric(ifstream, numTris);
                        ifstream.ignore(0xC - sizeof(numTris));
                        partBytesRead += 0xC - sizeof(numTris);

                        Matrix matrix{};
                        Point3D ptMin{}, ptMax{};
                        partBytesRead += readGeneric(ifstream, ptMin.x);
                        partBytesRead += readGeneric(ifstream, ptMin.y);
                        partBytesRead += readGeneric(ifstream, ptMin.z);
                        partBytesRead += readGeneric(ifstream, ptMin.w);

                        partBytesRead += readGeneric(ifstream, ptMax.x);
                        partBytesRead += readGeneric(ifstream, ptMax.y);
                        partBytesRead += readGeneric(ifstream, ptMax.z);
                        partBytesRead += readGeneric(ifstream, ptMax.w);

                        partBytesRead += readGeneric(ifstream, matrix);

                        BYTE unknownData[32];
                        partBytesRead += readGenericArray(ifstream, unknownData, sizeof(unknownData));

                        size_t nameLen = (size_t) partSize - 160 + 1;
                        char name[nameLen];
                        ifstream.read(name, nameLen - 1);
                        partBytesRead += nameLen - 1;

                        std::cout << "    Minimum point: " << ptMin.x << "/" << ptMin.y << "/" << ptMin.z << "/"
                                  << ptMin.w << std::endl;
                        std::cout << "    Maximum point: " << ptMax.x << "/" << ptMax.y << "/" << ptMax.z << "/"
                                  << ptMax.w << std::endl;
                        printf("%s\n", name);

                        break;
                    }
                    case bChunkID::BCHUNK_CARBON_MESH_HEADER:
                    {
                        partBytesRead += HandleGeometrySubChunkParts(ifstream, partId, partSize);

                        break;
                    }
                    case bChunkID::BCHUNK_CARBON_MATERIAL_NAME:
                    {
                        char name[partSize];
                        partBytesRead += readGenericArray(ifstream, name, sizeof(name));

                        printf("Material: %s\n", name);

                        break;
                    }
                    case bChunkID::BCHUNK_CARBON_MESH_FACES:
                    {
                        size_t numFaces = partSize / sizeof(tFace);

                        for (int j = 0; j < numFaces; j++)
                        {
                            tFace face{};
                            partBytesRead += readGeneric(ifstream, face);

                            printf("f %d %d %d\n", face.vA + 1, face.vB + 1, face.vC + 1);
                        }
                        break;
                    }
                    case bChunkID::BCHUNK_CARBON_MESH_VERTICES:
                    {
                        size_t numVertices = partSize / sizeof(tVertex);

                        std::vector<tVertex> vertices{};

                        for (int j = 0; j < numVertices; j++)
                        {
                            tVertex vertex{};
                            partBytesRead += readGeneric(ifstream, vertex);

                            if (vertex.x < -120250000 || vertex.x > 120250000 || std::isnan(vertex.x))
                            {
                                printf("x flag\n");
                                assert(j != 0);
                                vertex.x = vertices[j - 1].x;
                            }

                            if (vertex.y < -120250000 || vertex.y > 120250000 || std::isnan(vertex.y))
                            {
                                printf("y flag\n");
                                assert(j != 0);
                                vertex.y = vertices[j - 1].y;
                            }

                            if (vertex.z < -120250000 /* come on now... */ || vertex.z > 120250000 || std::isnan(vertex.z))
                            {
                                printf("z flag\n");
                                assert(j != 0);
                                vertex.z = vertices[j - 1].z;
                            }

                            printf("v %.4f %.4f %.4f\n", vertex.x, vertex.y, vertex.z);

                            vertices.emplace_back(vertex);
                        }
                        break;
                    }
                    default:
                        break;
                }

                dumpBytes(ifstream, partSize - partBytesRead);
                bytesRead += partBytesRead;
                ifstream.ignore(partSize - partBytesRead);
                bytesRead += partSize - partBytesRead;
            }

            return bytesRead;
        }

        void ParseGeometryChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            auto runTo = ((long) ifstream.tellg()) + size;

            size_t bytesRead = 0;

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < runTo; i++)
            {
                uint32_t scId, scSize;
                uint32_t scBytesRead = 0;

                bytesRead += readGeneric(ifstream, scId);
                bytesRead += readGeneric(ifstream, scSize);

                printf("    Subchunk: 0x%08x\n", scId);

                switch (scId)
                {
                    case 0x80134001:
                    case 0x80134010:
                    {
                        scBytesRead += HandleGeometrySubChunkParts(ifstream, scId, scSize);
                        break;
                    }
                    default:
                        break;
                }

                bytesRead += scBytesRead;
                ifstream.ignore(scSize - scBytesRead);
                bytesRead += scSize - scBytesRead;
            }

            ifstream.ignore(size - bytesRead);
        }
        // endregion

        void HandleFile(boost::filesystem::path &path, std::ifstream &ifstream, EAGLEye::FileType fileType)
        {
            switch (fileType)
            {
                case EAGLEye::COMPRESSED:
                    HandleCompressedFile(path, ifstream);
                    break;
                case EAGLEye::LOCATION_BASE:
                    HandleLocationBaseFile(path, ifstream);
                    break;
                default:
                    HandleRegularFile(path, ifstream);
                    break;
            }
        }

        void HandleLocationBaseFile(boost::filesystem::path &path, std::ifstream &ifstream)
        {
            std::cout << "[!] Handling base location file" << std::endl;
            HandleRegularFile(path, ifstream);

            boost::filesystem::path streamPath = path.parent_path();
            streamPath += "/STREAM";
            streamPath += path.filename();

            if (boost::filesystem::exists(streamPath))
            {
                std::cout << "[!] Handling map stream file" << std::endl;

                std::ifstream streamFileStream(streamPath.string(),
                                               std::ios::binary); // streamFileStream........ he comes... nOOOOoooooo
                HandleRegularFile(streamPath, streamFileStream);
            }
        }

        void HandleCompressedFile(boost::filesystem::path &path, std::ifstream &ifstream)
        {
            std::cout << "extreme compressed file xd" << std::endl;

            auto pos = (long) ifstream.tellg();

            {
                BYTE header[5];
                readGeneric(ifstream, header);
                assert(header[0] == 'J' && header[1] == 'D' && header[2] == 'L' && header[3] == 'Z' &&
                       header[4] == 0x02);
            }

            {
                BYTE t;
                readGeneric(ifstream, t);
                assert(t == 0x10);
            }

            {
                int16_t n;
                readGeneric(ifstream, n);
                assert(n == 0x0000);
            }

            size_t uncompressedLength, compressedLength;
            unsigned char uncompressedLength_[4];
            unsigned char compressedLength_[4];

            readGeneric(ifstream, uncompressedLength_);
            readGeneric(ifstream, compressedLength_);

            uncompressedLength = static_cast<size_t>(BitConverter::ToInt32(uncompressedLength_, 0));
            compressedLength = static_cast<size_t>(BitConverter::ToInt32(compressedLength_, 0));

            std::cout << uncompressedLength << "/" << compressedLength << std::endl;

            ifstream.seekg(pos);

            std::vector<BYTE> compressed;
            compressed.resize(compressedLength);

            ifstream.read((char *) &compressed[0], compressed.size());

            std::vector<BYTE> uncompressed = JDLZ::decompress(compressed);
            assert(uncompressed.size() == uncompressedLength);

            boost::filesystem::path decompressedPath = path;
            decompressedPath += ".tmp";

            {
                std::ofstream decompressedStream(decompressedPath.string(), std::ios::binary);
                decompressedStream.write((const char *) uncompressed.data(), uncompressedLength);
            }

            {
                std::ifstream decompressedStream(decompressedPath.string(), std::ios::binary);
                HandleRegularFile(decompressedPath, decompressedStream);
            }
        }

        void HandleRegularFile(boost::filesystem::path &path, std::ifstream &ifstream)
        {
            std::cout << "extreme regular file xd" << std::endl;

            uintmax_t fileSize = boost::filesystem::file_size(path);

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < fileSize; i++)
            {
                uint32_t id, size;
                readGeneric(ifstream, id);
                readGeneric(ifstream, size);

                auto chunkMapEntry = EAGLEye::chunkIdMap.find(id);

                if (chunkMapEntry == EAGLEye::chunkIdMap.end())
                {
                    printf("0x%08x - no info\n", id);
                } else
                {
                    printf("0x%08x - %s\n", id, chunkMapEntry->second.c_str());
                }

                switch (id)
                {
                    case bChunkID::BCHUNK_SPEED_ESOLID_LIST_CHUNKS:
                    {
                        ParseGeometryChunk(ifstream, id, size);
                        break;
                    }
                    default:
                    {
                        ifstream.ignore(size);
                        break;
                    }
                }
            }
        }
    }
}