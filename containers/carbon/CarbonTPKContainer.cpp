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

//                printf("    TPK Chunk #%02d: 0x%08x\n", i + 1, id);

                switch (id)
                {
                    case CARBON_TPK:
                    case CARBON_TPK_TEXTURE_DATA_ROOT:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case CARBON_TPK_TEXTURE_DATA_HASH:
                    {
                        m_stream.ignore(8);
                        bytesRead += 8;

                        int preNumber;
                        bytesRead += readGeneric(m_stream, preNumber);

                        int tpkHash;
                        bytesRead += readGeneric(m_stream, tpkHash);

//                        printf("preNumber = %d, tpkHash = 0x%08x\n", preNumber, tpkHash);
                        break;
                    }
                    case CARBON_TPK_TEXTURE_DATA_CONTAINER:
                    {
                        m_stream.ignore(0x78);
                        bytesRead += 0x78;

                        auto startPos = (long) m_stream.tellg();

                        for (auto &j : m_tpk->textures)
                        {
                            auto &texture = *j.second;

                            bytesRead += ((startPos + texture.dataOffset) - ((long) m_stream.tellg())); // what even?
                            m_stream.seekg(startPos + texture.dataOffset);

                            BYTE data[texture.dataSize];
                            bytesRead += readGenericArray(m_stream, data, sizeof(data));

                            std::ofstream ddsFile(texture.name + ".dds", std::ios::binary | std::ios::trunc);
                            tDDSHeader ddsHeader{};

                            ddsHeader.init(texture);
                            writeGeneric(ddsFile, ddsHeader, 0x80);

                            ddsFile.write((const char *) data, texture.dataSize);
                        }

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
                        }

                        break;
                    }
                    case CARBON_TPK_ANIMATED_TEXTURES:
                    {
                        m_tpk->animations.emplace_back(
                                std::make_shared<EAGLEye::Data::AnimatedTextureEntity>(EAGLEye::Data::AnimatedTextureEntity{}));
                    }
                    case CARBON_TPK_ANIMATED_TEXTURES_ROOT:
                    {
                        bytesRead += this->ReadChunks(size);
                        break;
                    }
                    case CARBON_TPK_ANIMATED_TEXTURES_ENTRY:
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

                        m_tpk->animations[m_tpk->animations.size() - 1]->name = std::string(name);
                        m_tpk->animations[m_tpk->animations.size() - 1]->fps = frameRate;
                        m_tpk->animations[m_tpk->animations.size() - 1]->numFrames = numFrames;

                        break;
                    }
                    case CARBON_TPK_ANIMATED_TEXTURES_HASHES:
                    {
                        for (int j = 0; j < m_tpk->animations[m_tpk->animations.size() - 1]->numFrames; j++)
                        {
                            int32_t hash;
                            bytesRead += readGeneric(m_stream, hash);

                            m_stream.ignore(8);
                            bytesRead += 8;

                            assert(std::find(m_tpk->hashTable.begin(), m_tpk->hashTable.end(), hash) !=
                                   m_tpk->hashTable.end());

                            m_tpk->animations[m_tpk->animations.size() - 1]->frameHashes.emplace_back(hash);
                        }

                        break;
                    }
                    case 0x33310005:
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
                    default:
//                        printf("    * Passing unhandled TPK chunk\n");
                        break;
                }

                assert(bytesRead <= size);

                totalBytesRead += size - bytesRead;
                m_stream.ignore(size - bytesRead);
                totalBytesRead += bytesRead;
            }

            return totalBytesRead;
        }

        void tDDSHeader::init(EAGLEye::Data::TextureInfo &info)
        {
            m_dwMagic = 0x20534444; //"DDS "
            m_dwSize = 0x7C;
            m_dwFlags = 0x81007;//DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_LINEARSIZE;
            m_dwHeight = info.height;
            m_dwWidth = info.width;
            m_dwPitchOrLinearSize = info.dataSize;
            //pixel format:
            m_PixelFormat.dwSize = 0x20;
            m_PixelFormat.dwFlags = ((info.ddsType & 0xFFFF) == 0x5844) ? 4//DDPF_FOURCC
                                                                        : 0;
            m_PixelFormat.dwFourCC = info.ddsType;
            if (0 == m_PixelFormat.dwFlags)
            {
                m_PixelFormat.dwRBitMask = 0xFF000000;
                m_PixelFormat.dwGBitMask = 0x00FF0000;
                m_PixelFormat.dwBBitMask = 0x0000FF00;
                m_PixelFormat.dwRGBAlphaBitMask = 0x000000FF;
            }
            //caps:
            m_ddsCaps.dwCaps1 = 0x1000;//DDSCAPS_TEXTURE;
        }
    }
}