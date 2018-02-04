#include "MW.h"
#include "../DataContext.h"
#include <iostream>
#include <algorithm>
#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/string/join.hpp>

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

        size_t HandleTPKSubchunkParts(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            size_t bytesRead = 0;
            auto runTo = ((long) ifstream.tellg()) + size;

            std::vector<int32_t> textureHashes{};

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
                    case ID_TPK_FILE:
                    {
                        uint32_t nMarker;
                        partBytesRead += readGeneric(ifstream, nMarker);

                        assert(HIWORD(nMarker) == 0x0000);

                        char name[0x1C];
                        partBytesRead += readGenericArray(ifstream, name, ARRAY_LENGTH(name));

                        char path[0x40];
                        partBytesRead += readGenericArray(ifstream, path, ARRAY_LENGTH(path));

                        printf("%s: %s\n", name, path);

                        for (int j = 0; j < 7; j++)
                        {
                            DWORD hash;
                            partBytesRead += readGeneric(ifstream, hash);
                        }

                        break;
                    }
                    case ID_TPK_TEXTURE_HASHES:
                    {
                        size_t numHashEntries = partSize / 8;

                        for (size_t j = 0; j < numHashEntries; j++)
                        {
                            int32_t hash;
                            partBytesRead += readGeneric(ifstream, hash);

                            uint32_t pad;
                            partBytesRead += readGeneric(ifstream, pad);

                            assert(pad == 0x00000000);
                            textureHashes.emplace_back(hash);
                        }

                        break;
                    }
                    case ID_TPK_TEXTURE_NAMES:
                    {
                        ifstream.ignore(0xC);
                        partBytesRead += 0xC;

                        std::cout << "Number of Textures: " << textureHashes.size() << std::endl;

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
//                        std::cout << geometryFileInfo.path << " [" << geometryFileInfo.section << "]" << std::endl;

                        break;
                    }

                    case ID_HASH_TABLE:
                    {
                        size_t numEntries = partSize / 8;
//                        std::cout << "Hashtable: " << numEntries << " entry(s)" << std::endl;

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

                        ifstream.ignore(12);
                        partBytesRead += 12; // 3 zeroes

                        uint32_t unk;
                        partBytesRead += readGeneric(ifstream, unk);

                        printf("unk1 = %d\n", unk);

                        uint32_t unk2;
                        partBytesRead += readGeneric(ifstream, unk2);

//                        ifstream.seekg(4, ifstream.cur);
//                        partBytesRead += 4;

                        long numTris;
                        partBytesRead += readGeneric(ifstream, numTris);
                        ifstream.ignore(0xC - sizeof(numTris));
                        partBytesRead += 0xC - sizeof(numTris);

                        Point3D ptMin{}, ptMax{};
                        Matrix matrix{};

                        partBytesRead += readGeneric(ifstream, ptMin);
                        partBytesRead += readGeneric(ifstream, ptMax);
                        partBytesRead += readGeneric(ifstream, matrix);

//                        hexdump(stdout, &matrix);

                        BYTE unknownData[32];
                        partBytesRead += readGenericArray(ifstream, unknownData, ARRAY_LENGTH(unknownData));
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
                                printf("type 1a\n");
                                ifstream.read((char *) &dwUnk4[1], 12);
                                headerSize += 0x10;
                            } else
                            {
                                printf("type 1b\n");
                                ifstream.seekg(-4, ifstream.cur);
                                partBytesRead -= 4;
                            }
                        } else
                        {
                            printf("type 2\n");
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
                        geometryItem.identifier = matrix.m[0];
                        geometryItem.name = std::string(name);
                        geometryItem.maxPoint = ptMax;
                        geometryItem.minPoint = ptMin;
                        catalog->items.emplace_back(std::make_unique<GeometryItem>(geometryItem));

                        break;
                    }
                    case ID_TEXTURE_USAGE:
                    {
//                        dumpBytes(ifstream, partSize);
//                        std::cout << "        Texture Usage" << std::endl;

                        long numTextures = partSize >> 3;

//                        std::cout << "        Textures: " << numTextures << std::endl;

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
//                        std::cout << "        Mesh Header" << std::endl;
//                        dumpBytes(ifstream, boost::algorithm::clamp(partSize, 0, 384));
                        partBytesRead += HandleGeometrySubchunkParts(ifstream, partId, partSize, catalog);

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
                    case ID_MATERIAL_NAME:
                    {
                        char name[partSize]; // literally, the name takes up the whole size
                        ifstream.read(name, partSize);
                        partBytesRead += partSize;

                        catalog->items[catalog->items.size() - 1]->mesh.materialNames.emplace_back(std::string(name));
                        break;
                    }
                    case ID_TEXTURE_APPLY:
                    {
//                        dumpBytes(ifstream, partSize);
                        break;
                    }
                    case ID_TRIANGLES:
                    {
                        for (int j = 0; j < partSize / sizeof(tFace); j++)
                        {
                            tFace face{};
                            partBytesRead += readGeneric(ifstream, face);

                            GeometryFace geoFace{};
                            geoFace.vA = face.vA;
                            geoFace.vB = face.vB;
                            geoFace.vC = face.vC;

                            printf("f %d %d %d\n", geoFace.vA + 1, geoFace.vB + 1, geoFace.vC + 1);

                            catalog->items[catalog->items.size() - 1]->mesh.faces.emplace_back(geoFace);
                        }

                        break;
                    }
                    case ID_VERTICES:
                    {
                        Point3D min = catalog->items[catalog->items.size() - 1]->minPoint, max = catalog->items[
                                catalog->items.size() - 1]->maxPoint;

                        if (catalog->filename == "GEOMETRY.BIN" && catalog->section == "DEFAULT")
                        {
                            printf("Seems to be a car\n");

                            size_t numVertices = partSize / sizeof(tCarVertex);

                            printf("Number of vertices: %zu\n", numVertices);

                            for (int j = 0; j < numVertices; j++)
                            {
                                tCarVertex carVertex{};
                                partBytesRead += readGeneric(ifstream, carVertex);

                                assert(carVertex.x >= min.x && carVertex.x <= max.x);
                                assert(carVertex.y >= min.y && carVertex.y <= max.y);
                                assert(carVertex.z >= min.z && carVertex.z <= max.z);
                                printf("%.4f/%.4f/%.4f\n", carVertex.x, carVertex.y, carVertex.z);
                            }
                        } else
                        {
                            size_t numVertices = partSize / sizeof(tWorldVertex);

                            printf("Number of vertices: %zu\n", numVertices);

                            for (int j = 0; j < numVertices; j++)
                            {
                                tWorldVertex vertex{};
                                partBytesRead += readGeneric(ifstream, vertex);

                                printf("v %.4f %.4f %.4f\n", vertex.x, vertex.y, vertex.z);
                            }
//                            size_t fixedSize = partSize;
//
//                            partBytesRead++;
//
//                            while (readByte(ifstream) == 0)
//                            {
//                                partBytesRead++;
//                            }
//
//                            partBytesRead--;
//                            ifstream.seekg(-1, ifstream.cur);
//
//                            fixedSize -= partBytesRead;
//
//                            assert(fixedSize <= partSize);

//                            for (size_t j = 0; j < boost::algorithm::clamp(data.size(), 0, 75); j++)
//                            {
//                                printf("float #%zu: %.4f\n", j, data[j]);
//                                hexdump(stdout, &data[j]);
//                            }



//                            dumpBytes(ifstream, boost::algorithm::clamp(partSize, 0, 512));
                        }

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
                        catalogs.push_back(std::make_unique<GeometryCatalog>(GeometryCatalog{}));
                    }
                    case 0x80134010:
                    {
                        assert(!catalogs.empty() && catalogs[catalogs.size() - 1] != nullptr);
                        scBytesRead += HandleGeometrySubchunkParts(ifstream, scId, scSize,
                                                                   catalogs[catalogs.size() - 1]);
                        break;
                    }
                    default:
                        break;
                }

                bytesRead += scBytesRead;
                ifstream.ignore(scSize - scBytesRead);
                bytesRead += scSize - scBytesRead;
            }

//            std::cout << "Catalogs: " << catalogs.size() << std::endl;

            for (auto &catalog : catalogs)
                GlobalData::catalogs.emplace_back(std::move(catalog));

            ifstream.ignore(size - bytesRead);

            return std::make_shared<EAGLEye::GeometryChunk>(EAGLEye::GeometryChunk{id, size});
        }

        void ParseTexturePackChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            size_t bytesRead = 0;
            auto runTo = ((long) ifstream.tellg()) + size;

            GeometryFileInfo_s geometryFileInfo{};
            std::vector<std::unique_ptr<GeometryCatalog>> catalogs{};

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < runTo; i++)
            {
                size_t scBytesRead = 0;
                uint32_t scId, scSize;

                bytesRead += readGeneric(ifstream, scId);
                bytesRead += readGeneric(ifstream, scSize);

                auto chunkMapEntry = EAGLEye::chunkIdMap.find(scId);

                if (chunkMapEntry == EAGLEye::chunkIdMap.end())
                {
                    printf("Sub-chunk #%d: 0x%08x - no info\n", i + 1, scId);
                } else
                {
                    printf("Sub-chunk #%d: 0x%08x - %s\n", i + 1, scId, chunkMapEntry->second.c_str());
                }

                switch (scId)
                {
                    case 0xb3310000:
                    {
                        scBytesRead += HandleTPKSubchunkParts(ifstream, scId, scSize);
                        break;
                    }
                    default:
                        break;
                }

//                printf("Sub-chunk #%d: 0x%08x - %s\n", i + 1, scId, EAGLEye::chunkIdMap.find(scId)->second.c_str());

                bytesRead += scBytesRead;
                ifstream.ignore(scSize - scBytesRead);
                bytesRead += scSize - scBytesRead;
            }

//            std::cout << "Catalogs: " << catalogs.size() << std::endl;

            for (auto &catalog : catalogs)
                GlobalData::catalogs.emplace_back(std::move(catalog));

            ifstream.ignore(size - bytesRead);
        }

        /**
         * generate an empty STREAML2RA
         */
        void GenerateStreamFile()
        {
            {
                std::ofstream stream("L2RA-inject.BUN", std::ios::binary);
                TrackStreamerSection_s trackStreamerSection{};

                trackStreamerSection.id[0] = 'X';
                trackStreamerSection.id[1] = '0';
                trackStreamerSection.Unk1 = 0;
                trackStreamerSection.StreamChunkNumber = 2400;
                trackStreamerSection.Unk2 = 0;
                trackStreamerSection.MasterStreamChunkNumber = 1;
                trackStreamerSection.MasterStreamChunkOffset = 0;
                trackStreamerSection.Size1 = 0;
                trackStreamerSection.Size2 = 0;
                trackStreamerSection.Size3 = 0;
                trackStreamerSection.X = 0;
                trackStreamerSection.Y = 0;
                trackStreamerSection.Z = 0;
                trackStreamerSection.StreamChunkHash = 0;

                memset(trackStreamerSection.data, '\0', sizeof(trackStreamerSection.data));

                writeGeneric(stream, 0x00034110);
                writeGeneric(stream, sizeof(trackStreamerSection));
                writeGeneric(stream, trackStreamerSection);
            }
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
                    case 0xb3300000: // BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS
                    {
                        ParseTexturePackChunk(ifstream, id, size);
                        break;
                    }
                    default:
                    {
                        ifstream.ignore(size);
                        break;
                    }
                }
            }


            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;

            std::cout << "Info - catalogs: " << GlobalData::catalogs.size() << std::endl;

            for (auto &catalog : GlobalData::catalogs)
            {
//                std::cout << "Catalog" << std::endl;
//                std::cout << "    Name:    " << catalog->filename << std::endl;
//                std::cout << "    Section: " << catalog->section << std::endl;
//                std::cout << "    Items: " << catalog->numItems << std::endl;

                boost::filesystem::path catalogPath = catalog->filename;
                catalogPath = catalogPath.stem();
                catalogPath = "data/modelsOut/" + catalogPath.string();

                catalogPath = boost::filesystem::absolute(catalogPath);

                if (!boost::filesystem::is_directory(catalogPath))
                    boost::filesystem::create_directories(catalogPath);

                for (auto &item : catalog->items)
                {
//                    std::cout << " Item" << std::endl;
//                    std::cout << "     Name:          " << item->name << std::endl;
//                    std::cout << "     Textures:      " << item->numTextures << std::endl;
//                    std::cout << "     Minimum Point: " << item->minPoint.x << "/" << item->minPoint.y << "/"
//                              << item->minPoint.z << std::endl;
//                    std::cout << "     Maximum Point: " << item->maxPoint.x << "/" << item->maxPoint.y << "/"
//                              << item->maxPoint.z << std::endl;
//                    std::cout << "     Vertices:      " << item->mesh.vertices.size() << std::endl;
//                    std::cout << "     Faces:         " << item->mesh.faces.size() << std::endl;
//                    std::cout << "     Materials:     " << boost::algorithm::join(item->mesh.materialNames, ", ")
//                              << std::endl;

                    {
                        boost::filesystem::path itemPath = catalogPath / (item->name + std::string(".obj"));

                        std::ofstream itemFile(itemPath.string(), std::ios::trunc);

                        itemFile << "# Generated by EAGLEye" << std::endl;
                        itemFile << "# Catalog: " << catalog->filename << " [" << catalog->section << "]" << std::endl;
                        itemFile << std::fixed;

                        itemFile << "# Materials [" << item->mesh.materialNames.size() << "]" << std::endl;

                        std::vector<GeometryVertex> writtenVertices;

                        for (auto &materialName : item->mesh.materialNames)
                        {
                            itemFile << "newmtl " << materialName << std::endl;
                            itemFile << "Ka 1.000 1.000 1.000" << std::endl;
                            itemFile << "Kd 1.000 1.000 1.000" << std::endl;
                            itemFile << "Ks 0.000 0.000 0.000" << std::endl;
                            itemFile << std::endl;
                        }

                        for (auto &materialName : item->mesh.materialNames)
                        {
                            itemFile << "usemtl " << materialName << std::endl;
                        }

                        itemFile << "g " << item->name << std::endl;

                        for (auto &vertex : item->mesh.vertices)
                        {
                            if (!vertex.good) continue;

                            itemFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
                            writtenVertices.emplace_back(vertex);
                        }

                        for (auto &face : item->mesh.faces)
                        {
                            if (face.vA >= writtenVertices.size() || face.vB >= writtenVertices.size() ||
                                face.vC >= writtenVertices.size())
                                continue;

                            itemFile << "f " << face.vA + 1 << " " << face.vB + 1 << " " << face.vC + 1 << std::endl;
                        }
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
