#include "MW.h"
#include "../DataContext.h"
#include <iostream>
#include <algorithm>
#include <boost/algorithm/clamp.hpp>

namespace EAGLEye
{
    namespace MW
    {
        MWBINChunk g_chunks[] = {
                {ID_FILE_HEADER,     0x00, "File Header"},
                {ID_DATA_HEADER,     0x00, "Data Header"}, //includes hash table
                {ID_BIN_ARCHIVE,     0x00, "BIN archive"},
                {ID_HASH_TABLE,      0x00, "Hash table"},
                {ID_OBJECTS_LAYOUT,  0x00, "hash mapping?"},
                {ID_BLANK,           0x00, "blank?"},
                {ID_ALIGN,           0x00, "align block?"},
                {ID_OBJECT,          0x00, "Object"},
                {ID_OBJECT_HEADER,   0x10, "Object header"},
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

        std::shared_ptr<EAGLEye::TrackPathChunk>
        ParseTrackPathChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            ifstream.ignore(size);

            return std::make_shared<EAGLEye::TrackPathChunk>(EAGLEye::TrackPathChunk{id, size});
        }

        size_t HandleGeometrySubchunkParts(std::ifstream &ifstream, uint32_t id, uint32_t size,
                                           std::unique_ptr<GeometryCatalog> &catalog)
        {
            size_t bytesRead = 0;
            auto runTo = ((long) ifstream.tellg()) + size;

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < runTo; i++)
            {
                uint32_t partId, partSize;
                size_t partBytesRead = 0;
                bytesRead += readGeneric(ifstream, partId);
                bytesRead += readGeneric(ifstream, partSize);

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

                printf("    > Part #%d: 0x%08x / 0x%06x -> %s\n", i + 1, partId, BIN_ID(partId),
                       g_chunks[nChunkIndex].m_pszType);

                switch (BIN_ID(partId))
                {
                    case ID_BIN_ARCHIVE:
                    {
                        GeometryFileInfo_s geometryFileInfo{};
                        partBytesRead += readGeneric(ifstream, geometryFileInfo);

                        catalog->filename = geometryFileInfo.path;
                        catalog->section = geometryFileInfo.section;
                        std::cout << geometryFileInfo.path << " [" << geometryFileInfo.section << "]" << std::endl;

                        break;
                    }

                    case ID_HASH_TABLE:
                    {
                        size_t numEntries = partSize / 8;
                        std::cout << "Hashtable: " << numEntries << " entry(s)" << std::endl;

                        for (size_t j = 0; j < numEntries; j++)
                        {
                            int32_t hash;
                            partBytesRead += readGeneric(ifstream, hash);
                            ifstream.ignore(4);
                            partBytesRead += 4;

                            catalog->hashTable.emplace_back(hash);
                        }

                        catalog->numItems = numEntries;

                        break;
                    }

                    case ID_OBJECT_HEADER:
                    {
//                        std::cout << "        Header" << std::endl;

                        ifstream.ignore(16);
                        partBytesRead += 16; // 3 zeroes and an unknown
                        ifstream.seekg(4, ifstream.cur);
                        partBytesRead += 4;

                        long numTris;
                        partBytesRead += readGeneric(ifstream, numTris);
                        ifstream.ignore(0xC - sizeof(numTris));
                        partBytesRead += 0xC - sizeof(numTris);

                        Point3D ptMin{}, ptMax{};
                        Matrix matrix{};

                        partBytesRead += readGeneric(ifstream, ptMin);
                        partBytesRead += readGeneric(ifstream, ptMax);
                        partBytesRead += readGeneric(ifstream, matrix);

                        ifstream.ignore(32);
                        partBytesRead += 32;
                        long headerSize = 160;

                        DWORD dwUnk4[6];
                        partBytesRead += readGeneric(ifstream, dwUnk4[0]);

                        if (dwUnk4[0] == 0)
                        {
                            ifstream.read((char *) &dwUnk4[1], 20);
                            partBytesRead += 20;
                            headerSize += 0x18;

                            partBytesRead += readGeneric(ifstream, dwUnk4[0]);

                            if (dwUnk4[0] == 0)
                            {
                                ifstream.read((char *) &dwUnk4[1], 12);
                                headerSize += 0x10;
                            } else
                            {
                                ifstream.seekg(-4, ifstream.cur);
                                partBytesRead -= 4;
                            }
                        } else
                        {
                            ifstream.seekg(-4, ifstream.cur);
                            partBytesRead -= 4;
                        }

                        size_t nameLen = (size_t) partSize - headerSize + 1;
                        char name[nameLen];
                        ifstream.read(name, nameLen - 1);
                        partBytesRead += nameLen - 1;
                        std::cout << "Geometry Item" << std::endl;
                        std::cout << "    Name: " << name << std::endl;
                        std::cout << "    Minimum point: " << ptMin.x << "/" << ptMin.y << "/" << ptMin.z << "/"
                                  << ptMin.w << std::endl;
                        std::cout << "    Maximum point: " << ptMax.x << "/" << ptMax.y << "/" << ptMax.z << "/"
                                  << ptMax.w << std::endl;

                        GeometryItem geometryItem{};
                        geometryItem.name = name;
                        geometryItem.maxPoint = ptMax;
                        geometryItem.minPoint = ptMin;
                        catalog->items.emplace_back(std::make_unique<GeometryItem>(geometryItem));

                        break;
                    }
                    case ID_TEXTURE_USAGE:
                    {
                        dumpBytes(ifstream, partSize);
//                        std::cout << "        Texture Usage" << std::endl;

                        long numTextures = partSize >> 3;

                        std::cout << "        Textures: " << numTextures << std::endl;

                        catalog->items[catalog->items.size() - 1]->numTextures = numTextures;

                        for (int j = 0; j < numTextures; j++)
                        {
                            int32_t hash;
                            partBytesRead += readGeneric(ifstream, hash);
                            ifstream.ignore(4);
                            partBytesRead += 4;
                        }

                        break;
                    }
                    case ID_MESH_HEADER:
                    {
                        std::cout << "        Mesh Header" << std::endl;
//                        dumpBytes(ifstream, boost::algorithm::clamp(partSize, 0, 384));
                        partBytesRead += HandleGeometrySubchunkParts(ifstream, partId, partSize, catalog);

                        catalog->items[catalog->items.size() - 1]->mesh = GeometryMesh{};

                        break;
                    }
                    case ID_MESH_DESCRIPTOR:
                    {
//                        std::cout << "        Mesh Descriptor" << std::endl;
                        break;
                    }
                    case ID_MAT_ASSIGN:
                    {
//                        std::cout << "        Material Assignment" << std::endl;
                        break;
                    }
                    case ID_TRIANGLES:
                    {
//                        std::cout << "        Faces" << std::endl;
//                        std::cout << "        " << partSize << " / " << sizeof(tFace) * (partSize / sizeof(tFace)) << std::endl;

                        for (size_t j = 0; j < partSize / sizeof(tFace); j++)
                        {
                            tFace face{};
                            partBytesRead += readGeneric(ifstream, face);

                            GeometryFace geoFace{};
                            geoFace.vA = face.vA;
                            geoFace.vB = face.vB;
                            geoFace.vC = face.vC;

                            catalog->items[
                                    catalog->items.size() - 1]->mesh.faces.emplace_back(geoFace);
//                            printf("f %d %d %d\n", face.vA + 1, face.vB + 1, face.vC + 1);
                        }

                        break;
                    }
                    case ID_VERTICES:
                    {
//                        std::cout << "        Vertices" << std::endl;
                        Point3D min = catalog->items[catalog->items.size() - 1]->minPoint, max = catalog->items[
                                catalog->items.size() - 1]->maxPoint;
                        auto faces = catalog->items[
                                catalog->items.size() - 1]->mesh.faces;

                        for (size_t j = 0; j < partSize / sizeof(tVertex); j++)
                        {
                            tVertex vertex{};
                            partBytesRead += readGeneric(ifstream, vertex);

                            if (((vertex.x < min.x || vertex.x > max.x) || (vertex.y < min.y || vertex.y > max.y) || (vertex.z < min.z || vertex.z > max.z)))
                            {
                                std::remove_if(faces.begin(), faces.end(), [&](GeometryFace& face)
                                {
                                    return face.vA == j || face.vB == j || face.vC == j;
                                });

                                continue;
                            }

                            GeometryVertex geoVertex{};
                            geoVertex.x = vertex.x;
                            geoVertex.y = vertex.y;
                            geoVertex.z = vertex.z;
                            geoVertex.u = vertex.u;
                            geoVertex.v = vertex.v;
                            catalog->items[
                                    catalog->items.size() - 1]->mesh.vertices.emplace_back(geoVertex);


//                            if (vertex.x > std::numeric_limits<float>::max() ||
//                                vertex.x < std::numeric_limits<float>::min() || std::isnan(vertex.x))
//                            {
//                                continue;
//                            }

//                            if (j % 5 == 0)
//                            {
//                                printf("entry %zu: %.4f\n", j, vertex.x);
//                            }

//                            printf("v %.4f %.4f %.4f\n", vertex.x, vertex.y, vertex.z);
                        }

                        catalog->items[
                                catalog->items.size() - 1]->mesh.faces = faces;

//                        std::cout << "        " << partSize << " / " << sizeof(tVertex) * (partSize / sizeof(tVertex)) << std::endl;
//                        float data[partSize >> 2];
//                        ifstream.read((char *) &data[0], sizeof(data));
//                        partBytesRead += sizeof(data);
//                        partBytesRead += readGeneric(ifstream, data);
//                        dumpBytes(ifstream, partSize - partBytesRead);

                        break;
                    }
                    case ID_MATERIAL_NAME:
                    {
                        char name[partSize]; // literally, the name takes up the whole size
                        ifstream.read(name, partSize);
                        partBytesRead += partSize;

                        catalog->items[catalog->items.size() - 1]->mesh.materialNames.emplace_back(std::string(name));

                        std::cout << "        Material: " << name << std::endl;
//                        std::cout << "        Material Name" << std::endl;
                        break;
                    }
                    default:
                        break;
                }

                bytesRead += partBytesRead;
                ifstream.ignore(partSize - partBytesRead);
                bytesRead += partSize - partBytesRead;
            }

            return bytesRead;
        }

        std::shared_ptr<EAGLEye::GeometryChunk> ParseGeometryChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            size_t bytesRead = 0;
            auto runTo = ((long) ifstream.tellg()) + size;

            GeometryFileInfo_s geometryFileInfo{};
            std::unique_ptr<GeometryCatalog> catalog{};
            std::vector<std::unique_ptr<GeometryCatalog>> catalogs{};

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < runTo; i++)
            {
                size_t scBytesRead = 0;
                uint32_t scId, scSize;

                bytesRead += readGeneric(ifstream, scId);
                bytesRead += readGeneric(ifstream, scSize);

                printf("Sub-chunk #%d: 0x%08x - %s\n", i + 1, scId, EAGLEye::chunkIdMap.find(scId)->second.c_str());

                switch (scId)
                {
                    case 0x80134001:
                    {
                        catalog = std::make_unique<GeometryCatalog>(GeometryCatalog{});
                    }
                    case 0x80134010:
                    {
                        assert(catalog != nullptr);
                        scBytesRead += HandleGeometrySubchunkParts(ifstream, scId, scSize, catalog);
                        break;
                    }
                    default:
                    {
                        if (catalog != nullptr)
                            catalogs.emplace_back(std::move(catalog));
                        catalog = nullptr;
                        break;
                    }
                }

                bytesRead += scBytesRead;
                ifstream.ignore(scSize - scBytesRead);
                bytesRead += scSize - scBytesRead;
            }

            ifstream.ignore(size - bytesRead);

            return std::make_shared<EAGLEye::GeometryChunk>(EAGLEye::GeometryChunk{id, size});
        }

        std::shared_ptr<EAGLEye::EAGLAnimationsChunk>
        ParseAnimationsChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            EAGLEye::EAGLAnimationsChunk chunk{id, size};
            size_t bytesRead = 0;
            AnimationsStruct_s animationsStruct_s{};

            bytesRead += readGeneric(ifstream, animationsStruct_s);

            dumpBytes(ifstream, 256);
            ifstream.ignore(size - bytesRead);

            return std::make_shared<EAGLEye::EAGLAnimationsChunk>(chunk);
        }

        std::shared_ptr<EAGLEye::TrackStreamerSectionsChunk>
        ParseTrackStreamerSectionsChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            EAGLEye::TrackStreamerSectionsChunk chunk{id, size};

            for (int i = 0; i < size / sizeof(EAGLEye::TrackStreamerSection_s); i++)
            {
                EAGLEye::TrackStreamerSection_s section{};
                readGeneric(ifstream, section);
                chunk.sections.emplace_back(section);
            }

            return std::make_shared<EAGLEye::TrackStreamerSectionsChunk>(chunk);
        }

        std::shared_ptr<EAGLEye::VisibleSectionChunk>
        ParseVisibleSectionChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            VisibleSectionStruct_s visibleSectionStruct{};
            readGeneric(ifstream, visibleSectionStruct);

            ifstream.ignore(size - sizeof(visibleSectionStruct));

            EAGLEye::VisibleSectionChunk chunk{id, size};

            chunk.visibleSection = visibleSectionStruct;

            std::cout << "Visible Section: " << visibleSectionStruct.name << std::endl;

            return std::make_shared<EAGLEye::VisibleSectionChunk>(chunk);
        }

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

//                printf("0x%08x - %s\n", id, EAGLEye::chunkIdMap.find(id)->second.c_str());

                switch (id)
                {
                    case 0x80134000: // 0x80134000
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
    }
}
