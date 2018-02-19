#include "UG2TPKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        UG2TPKContainer::UG2TPKContainer(std::ifstream &stream, uint32_t containerSize)
                : Container(stream),
                  m_containerSize(containerSize)
        {
        }

        std::shared_ptr<TPK> UG2TPKContainer::Get()
        {
            this->m_tpk = new EAGLEye::Data::TexturePack();
            this->ReadChunks(m_containerSize);

            return std::make_shared<TPK>(*this->m_tpk);
        }

        size_t UG2TPKContainer::ReadChunks(uint32_t totalSize)
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
                    case UG2_TPK:
                    case UG2_TPK_TEXTURE_DATA_ROOT:
                    {
                        bytesRead += this->ReadChunks(size);
//                        dumpBytes(m_stream, size);
                        break;
                    }
                    case UG2_TPK_PART_INFO:
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

                        printf("%s [%s | 0x%08x]\n", name, path, hash);

                        break;
                    }
                    case UG2_TPK_PART_HASHES:
                    {
                        size_t numEntries = size / 8; // 4-byte hash, 4-byte zero

                        for (int j = 0; j < numEntries; j++)
                        {
                            int32_t hash;
                            bytesRead += readGeneric(m_stream, hash);
                            m_stream.ignore(4);
                            bytesRead += 4;
                            m_tpk->hashTable.emplace_back(hash);

                            printf("Hash #%d: 0x%08x\n", j + 1, hash);
                        }

                        break;
                    }
                    case UG2_TPK_PART_HEADERS:
                    {
                        for (int j = 0; j < m_tpk->hashTable.size(); j++)
                        {
                            m_stream.ignore(0xC);
                            bytesRead += 0xC;

                            char name[24];
                            bytesRead += readGenericArray(m_stream, name, sizeof(name));

                            int32_t textureHash, typeHash;
                            uint32_t dataOffset, dataSize;

                            bytesRead += readGeneric(m_stream, textureHash);
                            bytesRead += readGeneric(m_stream, typeHash);

                            m_stream.ignore(4);
                            bytesRead += 4;

                            bytesRead += readGeneric(m_stream, dataOffset);

                            m_stream.ignore(4);
                            bytesRead += 4;

                            bytesRead += readGeneric(m_stream, dataSize);

                            assert(dataSize > 0); // Texture data cannot be blank!

                            m_stream.ignore(8);
                            bytesRead += 8;

                            BYTE data[56];
                            bytesRead += readGenericArray(m_stream, data, sizeof(data));

                            uint32_t resolution = BitConverter::ToUInt32(data, 0);
                            int32_t mipMap = BitConverter::ToInt32(data, 4);

                            printf("#%d: %s [0x%08x/0x%08x, do=0x%08x, ds=0x%08x] | %d by %d [mipmap %d]\n", j + 1, name, textureHash, typeHash, dataOffset, dataSize, LOWORD(resolution), HIWORD(resolution), HIWORD(mipMap));
                            hexdump(stdout, &data);

                            EAGLEye::Data::TextureInfo textureInfo{};
                            textureInfo.name = std::string(name);
                            textureInfo.hash = textureHash;
                            textureInfo.type = typeHash;
                            textureInfo.dataOffset = dataOffset;
                            textureInfo.dataSize = dataSize;
                            textureInfo.mipMap = HIWORD(mipMap);
                            textureInfo.width = LOWORD(resolution);
                            textureInfo.height = HIWORD(resolution);

                            m_tpk->textures[textureHash] = std::make_shared<EAGLEye::Data::TextureInfo>(textureInfo);
                        }
                        break;
                    }
                    case UG2_TPK_PART_DXT_HEADERS:
                    {
                        for (auto &texture : m_tpk->textures)
                        {
                            m_stream.seekg(0xC, m_stream.cur);
                            bytesRead += 0x0C;
                            bytesRead += readGeneric(m_stream, texture.second->ddsType, 0x4);
                            m_stream.seekg(0x08, m_stream.cur);
                            bytesRead += 0x08;

//                            printf("DDS Type: 0x%08x\n", texture.second->ddsType);
                        }
                        break;
                    }
//                    case UG2_TPK_TEXTURE_DATA_CONTAINER:
//                    {
//                        m_stream.ignore(0x78);
//                        bytesRead += 0x78;
//
//                        auto startPos = (long) m_stream.tellg();
//
//                        for (auto &j : m_tpk->textures)
//                        {
//                            auto &texture = *j.second;
//
//                            bytesRead += ((startPos + texture.dataOffset) - ((long) m_stream.tellg())); // what even?
//                            m_stream.seekg(startPos + texture.dataOffset);
//
//                            BYTE data[texture.dataSize];
//                            bytesRead += readGenericArray(m_stream, data, sizeof(data));
//
//                            std::ofstream ddsFile(texture.name + ".dds", std::ios::binary | std::ios::trunc);
//                            EAGLEye::Data::tDDSHeader ddsHeader{};
//
//                            ddsHeader.init(texture);
//                            writeGeneric(ddsFile, ddsHeader, 0x80);
//
//                            ddsFile.write((const char *) data, texture.dataSize);
//                        }
//                        break;
//                    }
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