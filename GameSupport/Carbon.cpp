#include "Carbon.h"
#include "../DataContext.h"
#include <iostream>
#include <algorithm>
#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/string/join.hpp>

namespace EAGLEye
{
    namespace Carbon
    {
        CarbonBINChunk g_chunks[] = {
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

                printf("        Part: 0x%08x\n", partId);

                switch (partId)
                {
                    case BCHUNK_CARBON_SOLIDS_ARCHIVE:
                    {
                        GeometryFileInfo_s geometryFileInfo{};
                        partBytesRead += readGeneric(ifstream, geometryFileInfo);

                        printf("%s [%s]\n", geometryFileInfo.path, geometryFileInfo.section);

                        break;
                    }
                    case BCHUNK_CARBON_SOLIDS_HASH_TABLE:
                    {
                        size_t numEntries = partSize / 8;

                        for (int j = 0; j < numEntries; j++)
                        {
                            int32_t hash;
                            partBytesRead += readGeneric(ifstream, hash);
                            ifstream.ignore(4);
                            partBytesRead += 4;
                        }

                        printf("Number of Objects: %zu\n", numEntries);
                        break;
                    }
                    case BCHUNK_CARBON_MESH_HEADER:
                    {
                        partBytesRead += HandleGeometrySubChunkParts(ifstream, partId, partSize);
                        break;
                    }
                    case BCHUNK_CARBON_SOLIDS_OBJECT_HEADER:
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
                    case BCHUNK_CARBON_MESH_DESCRIPTOR:
                    {
                        ifstream.ignore(40);
                        partBytesRead += 40;

                        long numTriangles;
                        partBytesRead += readGeneric(ifstream, numTriangles); // this doesn't actually work correctly

                        ifstream.ignore(4);
                        partBytesRead += 4;

                        break;
                    }
                    case BCHUNK_CARBON_MATERIAL_ASSIGN:
                    {
                        break;
                    }
                    case BCHUNK_CARBON_MATERIAL_NAME:
                    {
                        char name[partSize];
                        partBytesRead += readGenericArray(ifstream, name, sizeof(name));

                        printf("%s\n", name);
                        break;
                    }
                    case BCHUNK_CARBON_MESH_FACES:
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
                    case BCHUNK_CARBON_MESH_VERTICES:
                    {
                        size_t numVertices = partSize / sizeof(tVertex);

                        for (int j = 0; j < numVertices; j++)
                        {
                            tVertex vertex{};
                            partBytesRead += readGeneric(ifstream, vertex);

                            printf("v %f %f %f\n", vertex.x, vertex.y, vertex.z);
                        }

                        break;
                    }
                    default:
                        break;
                }

                if (partBytesRead > partSize)
                {
                    fprintf(stderr, "[!!!] Overran buffer! Read %zu bytes when we only have %d available\n",
                            partBytesRead, partSize);
                    exit(1);
                }

                dumpBytes(ifstream, boost::algorithm::clamp(partSize - partBytesRead, 0, 1024));

                bytesRead += partBytesRead;
                ifstream.ignore(partSize - partBytesRead);
                bytesRead += partSize - partBytesRead;
            }

            return bytesRead;
        }

        size_t HandleTPKSubChunkParts(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            auto runTo = ((long) ifstream.tellg()) + size;

            size_t bytesRead = 0;

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < runTo; i++)
            {
                uint32_t partId, partSize;
                size_t partBytesRead = 0;

                bytesRead += readGeneric(ifstream, partId);
                bytesRead += readGeneric(ifstream, partSize);

//                printf("        Part: 0x%08x\n", partId);

                switch (partId)
                {
                    case BCHUNK_CARBON_TPK_INFO: // TPK info and hash
                    {
//                        dumpBytes(ifstream, partSize);
                        BYTE nameMarker[4]; // 08 00 00 00
                        partBytesRead += readGenericArray(ifstream, nameMarker, ARRAY_LENGTH(nameMarker));

                        char name[0x1C];
                        char path[0x40];

                        partBytesRead += readGenericArray(ifstream, name, ARRAY_LENGTH(name));
                        partBytesRead += readGenericArray(ifstream, path, ARRAY_LENGTH(path));

                        int32_t hash;
                        partBytesRead += readGeneric(ifstream, hash);

                        printf("TPK: %s [%s | 0x%08x]\n", name, path, hash);
                        ifstream.ignore(24);
                        partBytesRead += 24;

//                        dumpBytes(ifstream, partSize - partBytesRead);

                        break;
                    }
                    case BCHUNK_CARBON_TPK_HASHES: // TPK texture hashes
                    {
                        size_t numEntries = partSize / 8; // 4-byte hash, 4-byte zero

                        for (int j = 0; j < numEntries; j++)
                        {
                            int32_t hash;
                            partBytesRead += readGeneric(ifstream, hash);
                            ifstream.ignore(4);
                            partBytesRead += 4;

//                            printf("Hash #%d: 0x%08x\n", j + 1, hash);
                        }

//                        dumpBytes(ifstream, partSize - partBytesRead);

                        break;
                    }
                    case BCHUNK_CARBON_TPK_TEXTURE_NAMES: // TPK texture names
                    {
                        ifstream.ignore(0xC);
                        partBytesRead += 0xC;

//                        dumpBytes(ifstream, partSize - partBytesRead);

                        break;
                    }
                    default:
                        break;
                }

//                dumpBytes(ifstream, scSize);

                bytesRead += partBytesRead;
                ifstream.ignore(partSize - partBytesRead);
                bytesRead += partSize - partBytesRead;
            }

            return bytesRead;
        }

        void ParseTPKChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            size_t bytesRead = 0;

            auto runTo = ((long) ifstream.tellg()) + size;

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < runTo; i++)
            {
                uint32_t scId, scSize;
                uint32_t scBytesRead = 0;

                bytesRead += readGeneric(ifstream, scId);
                bytesRead += readGeneric(ifstream, scSize);

                printf("    Subchunk: 0x%08x\n", scId);

                switch (scId)
                {
                    case bChunkID::BCHUNK_CARBON_TPK_CONTAINER:
                    {
                        scBytesRead += HandleTPKSubChunkParts(ifstream, scId, scSize);
                        break;
                    }
                    default:
                        break;
                }

//                dumpBytes(ifstream, scSize);

                bytesRead += scBytesRead;
                ifstream.ignore(scSize - scBytesRead);
                bytesRead += scSize - scBytesRead;
            }

            ifstream.ignore(size - bytesRead);
        }

        void ParseGeometryChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
//            dumpBytes(ifstream, boost::algorithm::clamp(size, 0, 50000));

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

//                dumpBytes(ifstream, scSize);

                bytesRead += scBytesRead;
                ifstream.ignore(scSize - scBytesRead);
                bytesRead += scSize - scBytesRead;
            }

            ifstream.ignore(size - bytesRead);
        }

        void ParseSceneryChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
//            dumpBytes(ifstream, boost::algorithm::clamp(size, 0, 50000));

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
                    case BCHUNK_CARBON_SCENERY_NAMES:
                    {
                        size_t numEntries = scSize / 72;

                        printf("Number of entries: %zu\n", numEntries);

                        for (int j = 0; j < numEntries; j++)
                        {
                            char name[24];
                            scBytesRead += readGenericArray(ifstream, name, ARRAY_LENGTH(name));

//                            printf("%s\n", name);
                            ifstream.ignore(48);
                            scBytesRead += 48;
                        }

                        break;
                    }
                    default:
                        break;
                }

                dumpBytes(ifstream, boost::algorithm::clamp(scSize - scBytesRead, 0, 128));

                bytesRead += scBytesRead;
                ifstream.ignore(scSize - scBytesRead);
                bytesRead += scSize - scBytesRead;
            }

            ifstream.ignore(size - bytesRead);
        }

        void ParseELightsChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            MaterialDefinition materialDefinition{};
            readGeneric(ifstream, materialDefinition);

            printf("%s [0x%08x]\n", materialDefinition.name, materialDefinition.hash);
        }

        void ParseEventSequenceChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
//            dumpBytes(ifstream, boost::algorithm::clamp(size, 0, 50000));

            auto runTo = ((long) ifstream.tellg()) + size;

            size_t bytesRead = 0;

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < runTo; i++)
            {
                uint32_t scId, scSize;
                uint32_t scBytesRead = 0;

                bytesRead += readGeneric(ifstream, scId);
                bytesRead += readGeneric(ifstream, scSize);

                printf("    Subchunk: 0x%08x\n", scId);

                ifstream.ignore(24);
                scBytesRead += 24;

                switch (scId)
                {
                    case 0x0003b811: // PRAC
                    {
                        char pracID[4];
                        scBytesRead += readGenericArray(ifstream, pracID, ARRAY_LENGTH(pracID));

                        assert(pracID[0] == 'P' && pracID[1] == 'R' && pracID[2] == 'A' && pracID[3] == 'C');

                        ifstream.ignore(12);
                        scBytesRead += 12;

                        char srtSID[4];
                        scBytesRead += readGenericArray(ifstream, srtSID, ARRAY_LENGTH(srtSID));

                        assert(srtSID[0] == 's' && srtSID[1] == 'r' && srtSID[2] == 't' && srtSID[3] == 'S');

                        break;
                    }
                    default:
                        break;
                }

//                dumpBytes(ifstream, scSize - scBytesRead);

                bytesRead += scBytesRead;
                ifstream.ignore(scSize - scBytesRead);
                bytesRead += scSize - scBytesRead;
            }

            ifstream.ignore(size - bytesRead);
        }

        void ParseCarInfoArrayChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            size_t bytesRead = 0;
            auto runTo = ((long) ifstream.tellg()) + size;

            bytesRead++;

            while (readByte(ifstream) == 0x11)
            {
                bytesRead++;
            }

            bytesRead--;
            ifstream.seekg(-1, ifstream.cur);

            while (ifstream.tellg() < runTo)
            {
                char id1[16];
                bytesRead += readGenericArray(ifstream, id1, ARRAY_LENGTH(id1));
                char id2[16];
                bytesRead += readGenericArray(ifstream, id2, ARRAY_LENGTH(id2));
                char path[32];
                bytesRead += readGenericArray(ifstream, path, ARRAY_LENGTH(path));
                char maker[16];
                bytesRead += readGenericArray(ifstream, maker, ARRAY_LENGTH(maker));

                // 208 bytes total
                // id1+id2+path+maker=(16*2)+32+16=80
                // 208-80 = 128

                int32_t nameHash;
                bytesRead += readGeneric(ifstream, nameHash);

                ifstream.ignore(16);
                bytesRead += 16;

                ifstream.ignore(44);
                bytesRead += 44;

                BYTE carId;
                bytesRead += readGeneric(ifstream, carId);

                ifstream.ignore(7);
                bytesRead += 7;

                int32_t typeHash;
                bytesRead += readGeneric(ifstream, typeHash);

                ifstream.ignore(42);
                bytesRead += 42;

                BYTE unk1, skinsDisabled;
                bytesRead += readGeneric(ifstream, unk1);
                bytesRead += readGeneric(ifstream, skinsDisabled);

                ifstream.ignore(4);
                bytesRead += 4;

                int32_t reflectionConf; // ???
                bytesRead += readGeneric(ifstream, reflectionConf);

                // MUSTANGGT

                printf("#%d: %s %s [%s/0x%08x]: %s\n", carId + 1, maker, id1, id2, nameHash, path);
                printf("    Type = 0x%08x | Skins disabled = %s | Unknown material config = 0x%08x\n", typeHash,
                       skinsDisabled == 1 ? "Yes" : "No", reflectionConf);
            }

//            dumpBytes(ifstream, size - bytesRead);
            ifstream.ignore(size - bytesRead);
        }

        void ParseCarGeometryChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            ifstream.ignore(size);
        }

        void ParseFNGChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            size_t bytesRead = 0;

            auto headerPos = (long) ifstream.tellg();
            auto runTo = (long) ifstream.tellg() + size;

            char fenID[4];
            bytesRead += readGenericArray(ifstream, fenID, sizeof(fenID));

            assert(fenID[0] == 'F' && fenID[1] == 'E' && fenID[2] == 'n');

            ifstream.ignore(36);
            bytesRead += 36;

            std::stringstream fngFileName;
            std::stringstream fngFilePath;
            char c;

            while (true)
            {
                bytesRead++;

                if ((c = readByte(ifstream)) == 0)
                {
                    break;
                }

                fngFileName << c;
            }

            while (true)
            {
                bytesRead++;

                if ((c = readByte(ifstream)) == 0)
                {
                    break;
                }

                fngFilePath << c;
            }

            printf("New FNG: %s [%s]\n", fngFileName.str().c_str(), fngFilePath.str().c_str());
//            printf("tracking point 1 [%s | %s]: %zu bytes read out of %d bytes\n", fngFileName.str().c_str(), fngFilePath.str().c_str(), bytesRead, size);

            bytesRead -= ((long) ifstream.tellg()) - headerPos;
            ifstream.seekg(headerPos);

            while (ifstream.tellg() < runTo)
            {
                BYTE FNGtempSAT[4];
                bytesRead += readGenericArray(ifstream, FNGtempSAT, sizeof(FNGtempSAT));

                if ((FNGtempSAT[0] == 'S' && FNGtempSAT[1] == 'A')
                    || (FNGtempSAT[0] == 0xFF && FNGtempSAT[1] == 0xFF && FNGtempSAT[2] == 0xFF &&
                        FNGtempSAT[3] == 0xFF)) // Check if SA(0x90), SAD, SAT
                {
                    int FNGtempBlue, FNGtempGreen, FNGtempRed, FNGtempAlpha;
                    bytesRead += readGeneric(ifstream, FNGtempBlue);
                    bytesRead += readGeneric(ifstream, FNGtempGreen);
                    bytesRead += readGeneric(ifstream, FNGtempRed);
                    bytesRead += readGeneric(ifstream, FNGtempAlpha);

//                    printf("tracking point: %zu bytes read out of %d bytes\n", bytesRead, size);

                    if ((FNGtempBlue >= 0 && FNGtempBlue <= 255)
                        && (FNGtempGreen >= 0 && FNGtempGreen <= 255)
                        && (FNGtempRed >= 0 && FNGtempRed <= 255)
                        && (FNGtempAlpha >= 0 && FNGtempAlpha <= 255))
                    {
                        printf("%d/%d/%d/%d\n", FNGtempRed, FNGtempGreen, FNGtempBlue, FNGtempAlpha);
                    }
                }
            }

            if (bytesRead > size)
            {
                printf("!!! OVERRAN CHUNK: read %zu bytes when we only have %d bytes available\n", bytesRead, size);
                exit(1);
            }

            dumpBytes(ifstream, size - bytesRead);
            ifstream.ignore(size - bytesRead);
        }

        void ParseLanguageChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            size_t bytesRead = 0;

            LanguageFileHeader_s languageFileHeader{};
            bytesRead += readGeneric(ifstream, languageFileHeader);

            printf("%s - %d string(s)\n", languageFileHeader.name, languageFileHeader.numberOfStrings);

            for (int i = 0; i < languageFileHeader.numberOfStrings; i++)
            {
                LanguageEntry_s languageEntry{};
                bytesRead += readGeneric(ifstream, languageEntry);
            }

            for (int i = 0; i < languageFileHeader.numberOfStrings; i++)
            {
                std::stringstream textStream;

                BYTE rb;
                bytesRead += readGeneric(ifstream, rb);

                while (rb != 0x00)
                {
                    if (rb >= 32 && rb < 127)
                        textStream << rb;
                    else
                        textStream << ' ';
                    bytesRead += readGeneric(ifstream, rb);
                }

                printf("#%d: %s\n", i + 1, textStream.str().c_str());
            }

            dumpBytes(ifstream, size - bytesRead);
            ifstream.ignore(size - bytesRead);

//            dumpBytes(ifstream, size);
//            ifstream.ignore(size);
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

        void HandleCarFile(boost::filesystem::path &path, std::ifstream &ifstream)
        {
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
                        ParseCarGeometryChunk(ifstream, id, size);
                        break;
                    }
                    default:
                        ifstream.ignore(size);
                }
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

        void ParseAnimation(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
//            dumpBytes(ifstream, size);

            ifstream.ignore(8);
            uint32_t numTrees;
            readGeneric(ifstream, numTrees);
            ifstream.ignore(12);

            printf("Number of Trees: %d\n", numTrees);

            for (int i = 0; i < numTrees; i++)
            {
                uint32_t trId, trSize;
                readGeneric(ifstream, trId);
                readGeneric(ifstream, trSize);

                assert(trId == bChunkID::BCHUNK_SPEED_BBGANIM_INSTANCE_TREE);

                while (readByte(ifstream) == 0x11)
                {}

                ifstream.seekg(-1, ifstream.cur);

                uint32_t numNodes;
                readGeneric(ifstream, numNodes);

                printf("    Number of Nodes: %d\n", numNodes);

                for (int j = 0; j < numNodes; j++)
                {
                    uint32_t nId, nSize;
                    readGeneric(ifstream, nId);
                    readGeneric(ifstream, nSize);

                    assert(nId == bChunkID::BCHUNK_SPEED_BBGANIM_INSTANCE_NODE);

                    ifstream.ignore(nSize);
                }

                uint32_t ephId, ephSize;
                readGeneric(ifstream, ephId);
                readGeneric(ifstream, ephSize);

                if (ephId == 0x00037240) // unknown giant block; research more
                {
                    ifstream.ignore(ephSize);
                    readGeneric(ifstream, ephId);
                    readGeneric(ifstream, ephSize);
                }

                assert(ephId == bChunkID::BCHUNK_SPEED_BBGANIM_ENDPACKHEADER);

                ifstream.ignore(ephSize);
            }
//
//
//            ifstream.ignore(size);
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
                    case bChunkID::BCHUNK_CARINFO_ARRAY:
                    {
                        ParseCarInfoArrayChunk(ifstream, id, size);
                        break;
                    }
                    case bChunkID::BCHUNK_SPEED_ESOLID_LIST_CHUNKS:
                    {
                        ParseGeometryChunk(ifstream, id, size);
                        break;
                    }
                    case bChunkID::BCHUNK_EVENTSEQUENCE:
                    {
                        ParseEventSequenceChunk(ifstream, id, size);
                        break;
                    }
                    case bChunkID::BCHUNK_ELIGHTS:
                    {
                        ParseELightsChunk(ifstream, id, size);
                        break;
                    }
                    case bChunkID::BCHUNK_SPEED_TEXTURE_PACK_LIST_CHUNKS:
                    {
                        ParseTPKChunk(ifstream, id, size);
                        break;
                    }
                    case bChunkID::BCHUNK_SPEED_SCENERY_SECTION:
                    {
                        ParseSceneryChunk(ifstream, id, size);
                        break;
                    }
                    case bChunkID::BCHUNK_SPEED_BBGANIM_BLOCKHEADER:
                    {
                        ParseAnimation(ifstream, id, size);
                        break;
                    }
                    case bChunkID::BCHUNK_LANGUAGE:
                    {
                        ParseLanguageChunk(ifstream, id, size);
                        break;
                    }
                    case bChunkID::BCHUNK_FENG_PACKAGE:
                    {
                        ParseFNGChunk(ifstream, id, size);
                        break;
                    }
//                    case bChunkID::BCHUNK_EAGLSKELETONS:
//                    {
//                        dumpBytes(ifstream, size);
//                    }
//                    case bChunkID::BCHUNK_SPEED_BBGANIM_INSTANCE_TREE:
//                    {
//                        dumpBytes(ifstream, size);
//                    }
//                    case bChunkID::BCHUNK_SPEED_BBGANIM_INSTANCE_NODE:
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