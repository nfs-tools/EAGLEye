#include "CarbonTPKContainer.h"
#include <boost/algorithm/clamp.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        CarbonTPKContainer::CarbonTPKContainer(std::ifstream &stream, uint32_t containerSize) :
                Container(stream),
                m_containerSize(containerSize)
        {
        }

        std::shared_ptr<TPK> CarbonTPKContainer::Get()
        {
            this->m_tpk = new EAGLEye::Data::TexturePack();
            this->ReadChunks(m_containerSize);

            return std::make_shared<TPK>(*m_tpk);
        }

        size_t CarbonTPKContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

                size_t bytesRead = 0;

                printf("    TPK Chunk #%02d: 0x%08x\n", i + 1, id);

                switch (id)
                {
                    case CARBON_TPK:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case CARBON_TPK_PART_INFO:
                    {
                        m_stream.ignore(4);
                        bytesRead += 4;

                        char name[0x1C];
                        bytesRead += readGenericArray(m_stream, name, sizeof(name));

                        char path[0x40];
                        bytesRead += readGenericArray(m_stream, path, sizeof(path));

                        int32_t hash;
                        bytesRead += readGeneric(m_stream, hash);

                        m_stream.ignore(24);
                        bytesRead += 24;

                        m_tpk->name = std::string(name);
                        m_tpk->tpkPath = std::string(path);
                        m_tpk->hash = hash;

                        dumpBytes(m_stream, size - bytesRead);
                        break;
                    }
                    case CARBON_TPK_PART_HASHES:
                    {
                        size_t numEntries = size / 8; // 4-byte hash, 4-byte zero

                        for (int j = 0; j < numEntries; j++)
                        {
                            int32_t hash;
                            bytesRead += readGeneric(m_stream, hash);
                            m_stream.ignore(4);
                            bytesRead += 4;
                            m_tpk->hashTable.emplace_back(hash);
                        }

                        break;
                    }
                    case CARBON_TPK_PART_HEADERS:
                    {
                        for (int j = 0; j < m_tpk->hashTable.size(); j++)
                        {
                            BYTE data[0x58];
                            bytesRead += readGenericArray(m_stream, data, sizeof(data));
                            BYTE nameLength = readByte(m_stream);
                            bytesRead++;
                            char name[nameLength];
                            bytesRead += readGenericArray(m_stream, name, sizeof(name));
                        }

                        dumpBytes(m_stream, size - bytesRead);

                        break;
                    }
                    case CARBON_TPK_ANIMATED_TEXTURES:
                    case CARBON_TPK_ANIMATED_TEXTURES_ROOT:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case CARBON_TPK_ANIMATED_TEXTURES_ENTRIES:
                    {
                        bytesRead += 8;
                        m_stream.ignore(8);

                        char name[0x10];
                        bytesRead += readGenericArray(m_stream, name, sizeof(name));

                        int hash;
                        bytesRead += readGeneric(m_stream, hash);

                        BYTE numFrames = readByte(m_stream), frameRate = readByte(m_stream);
                        m_stream.ignore(2);
                        bytesRead += 4;
                        m_stream.ignore(0xC);
                        bytesRead += 0xC;

                        printf("%s - 0x%08x; %d frames @ %d fps\n", name, hash, numFrames, frameRate);

                        dumpBytes(m_stream, size - bytesRead);
                        break;
                    }
                    case CARBON_TPK_ANIMATED_TEXTURES_HASHES:
                    {
                        int entries = size / 12;

                        for (int j = 0; j < entries; j++)
                        {
                            int hash;
                            bytesRead += readGeneric(m_stream, hash);

                            m_stream.ignore(8);
                            bytesRead += 8;

                            printf("Frame #%d hash: 0x%08x\n", j + 1, hash);
                        }

                        break;
                    }
                    default:
                        printf("    * Passing unhandled TPK chunk\n");
                        break;
                }

                assert(bytesRead <= size);

                totalBytesRead += bytesRead;
                m_stream.ignore(size - bytesRead);
                totalBytesRead += size - bytesRead;
            }

            return totalBytesRead;
        }

//        std::shared_ptr<TPK> CarbonTPKContainer::ReadData()
//        {
//            auto runTo = (long) m_stream.tellg() + m_containerSize;
//
//            this->m_tpk = new EAGLEye::Data::TexturePack();
//
//            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
//            {
//                uint32_t id, size;
//                readGeneric(m_stream, id);
//                readGeneric(m_stream, size);
//
//                size_t bytesRead = 0;
//
//                printf("    TPK Chunk #%02d: 0x%08x\n", i + 1, id);
//
//                switch (id)
//                {
//                    case CARBON_TPK:
//                    {
//                        bytesRead += ReadChunkParts(size);
//                        break;
//                    }
//                    default:
//                        printf("    * Passing unhandled TPK chunk\n");
//                        break;
//                }
//
//                m_stream.ignore(size - bytesRead);
//            }
//
//            return std::make_shared<TPK>(*this->m_tpk);
//        }
//
//        size_t CarbonTPKContainer::ReadChunkParts(uint32_t totalSize)
//        {
//            auto runTo = (long) m_stream.tellg() + totalSize;
//            size_t totalBytesRead = 0;
//
//            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
//            {
//                uint32_t id, size;
//                totalBytesRead += readGeneric(m_stream, id);
//                totalBytesRead += readGeneric(m_stream, size);
//
//                size_t bytesRead = 0;
//
//                printf("        TPK Chunk Part #%02d: 0x%08x\n", i + 1, id);
//
//                switch (id)
//                {
//                    case CARBON_TPK_PART_INFO:
//                    {
//                        m_stream.ignore(4);
//                        bytesRead += 4;
//
//                        char name[0x1C];
//                        bytesRead += readGenericArray(m_stream, name, sizeof(name));
//
//                        char path[0x40];
//                        bytesRead += readGenericArray(m_stream, path, sizeof(path));
//
//                        int32_t hash;
//                        bytesRead += readGeneric(m_stream, hash);
//
//                        m_stream.ignore(24);
//                        bytesRead += 24;
//
//                        m_tpk->name = std::string(name);
//                        m_tpk->tpkPath = std::string(path);
//                        m_tpk->hash = hash;
//
//                        dumpBytes(m_stream, size - bytesRead);
//                        break;
//                    }
//                    case CARBON_TPK_PART_HASHES:
//                    {
//                        size_t numEntries = size / 8; // 4-byte hash, 4-byte zero
//
//                        for (int j = 0; j < numEntries; j++)
//                        {
//                            int32_t hash;
//                            bytesRead += readGeneric(m_stream, hash);
//                            m_stream.ignore(4); bytesRead += 4;
//                            m_tpk->hashTable.emplace_back(hash);
//                        }
//
//                        break;
//                    }
//                    default:
//                        printf("        * Passing unhandled TPK chunk part\n");
//                        break;
//                }
//
//                totalBytesRead += bytesRead;
//                m_stream.ignore(size - bytesRead);
//                totalBytesRead += size - bytesRead;
//            }
//
//            return totalBytesRead;
//        }
    }
}