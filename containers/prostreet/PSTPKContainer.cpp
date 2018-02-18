#include "PSTPKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        PSTPKContainer::PSTPKContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {
        }

        std::shared_ptr<TPK> PSTPKContainer::Get()
        {
            this->m_tpk = new EAGLEye::Data::TexturePack();
            this->ReadChunks(m_containerSize);

            return std::make_shared<TPK>(*this->m_tpk);
        }

        size_t PSTPKContainer::ReadChunks(uint32_t totalSize)
        {
            auto runTo = (long) m_stream.tellg() + totalSize;
            size_t totalBytesRead = 0;

            for (int i = 0; i < 0xFFFF && m_stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                totalBytesRead += readGeneric(m_stream, id);
                totalBytesRead += readGeneric(m_stream, size);

                size_t bytesRead = 0;

                printf("    TPK Chunk #%02d: 0x%08x @ 0x%08lx\n", i + 1, id, (unsigned long) m_stream.tellg());

                switch (id)
                {
                    case PS_TPK:
                    case PS_TPK_TEXTURE_DATA_ROOT:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case PS_TPK_PART_INFO:
                    {
                        PSTPKHeader tpkHeader{};
                        bytesRead += readGeneric(m_stream, tpkHeader);

                        m_tpk->name = std::string(tpkHeader.name);
                        m_tpk->tpkPath = std::string(tpkHeader.path);
                        m_tpk->hash = tpkHeader.hash;

                        printf("%s [%s / 0x%08x]\n", tpkHeader.name, tpkHeader.path, tpkHeader.hash);

                        break;
                    }
                    case PS_TPK_PART_HASHES:
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
                    case PS_TPK_PART_HEADERS:
                    {
                        for (int j = 0; j < m_tpk->hashTable.size(); j++)
                        {
                            m_stream.ignore(12);
                            bytesRead += 12;

                            BYTE data[0x58 - 12];
                            bytesRead += readGenericArray(m_stream, data, sizeof(data));

                            int32_t hash = BitConverter::ToInt32(data, 0);
                            int32_t hash2 = BitConverter::ToInt32(data, 4);
                            uint32_t dataOffset = BitConverter::ToUInt32(data, 8);
                            uint32_t dataSize = BitConverter::ToUInt32(data, 16);
                            uint32_t resolution = BitConverter::ToUInt32(data, 28);
                            int32_t mipMap = BitConverter::ToInt32(data, 32);

                            BYTE nameLength = readByte(m_stream);
                            bytesRead++;
                            char name[nameLength];
                            bytesRead += readGenericArray(m_stream, name, sizeof(name));

                            EAGLEye::Data::TextureInfo textureInfo{};
                            textureInfo.name = std::string(name);
                            textureInfo.hash = hash;
                            textureInfo.type = hash2;
                            textureInfo.dataOffset = dataOffset;
                            textureInfo.dataSize = dataSize;
                            textureInfo.mipMap = HIWORD(mipMap);
                            textureInfo.width = LOWORD(resolution);
                            textureInfo.height = HIWORD(resolution);

                            m_tpk->textures[hash] = std::make_shared<EAGLEye::Data::TextureInfo>(textureInfo);

                            printf("#%d: %s [0x%08x/0x%08x] | do/ds = 0x%08x/0x%08x | w/h = %d by %d | mipmap = %d\n",
                                   j + 1, name, hash, hash2, dataOffset, dataSize, LOWORD(resolution),
                                   HIWORD(resolution), HIWORD(mipMap));
                        }
                        break;
                    }
                    case PS_TPK_PART_DXT_HEADERS:
                    {
                        for (auto &texture : m_tpk->textures)
                        {
                            m_stream.seekg(0xC, m_stream.cur);
                            bytesRead += 0x0C;
                            bytesRead += readGeneric(m_stream, texture.second->ddsType, 0x4);
                            m_stream.seekg(0x08, m_stream.cur);
                            bytesRead += 0x08;
                        }

                        break;
                    }

                    case PS_TPK_TEXTURE_DATA_CONTAINER:
                    {
                        auto maxPos = (long) m_stream.tellg() + size;

                        m_stream.ignore(0x78);
                        bytesRead += 0x78;

                        auto startPos = (long) m_stream.tellg();

                        for (auto &j : m_tpk->textures)
                        {
                            auto &texture = *j.second;

                            printf("[TPK] Reading texture %s [0x%08x]\n", texture.name.c_str(), texture.hash);

                            bytesRead += ((startPos + texture.dataOffset) -
                                          ((long) m_stream.tellg())); // what even?
                            m_stream.seekg(startPos + texture.dataOffset);

                            BYTE data[texture.dataSize];
                            bytesRead += readGenericArray(m_stream, data, sizeof(data));

//                                std::ofstream ddsFile(texture.name + ".dds", std::ios::binary | std::ios::trunc);
//                                EAGLEye::Data::tDDSHeader ddsHeader{};
//
//                                ddsHeader.init(texture);
//                                writeGeneric(ddsFile, ddsHeader, 0x80);
//
//                                ddsFile.write((const char *) data, texture.dataSize);
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