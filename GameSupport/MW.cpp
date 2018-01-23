#include "MW.h"
#include <iostream>

namespace EAGLEye
{
    namespace MW
    {
        std::shared_ptr<EAGLEye::TrackPathChunk>
        ParseTrackPathChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            ifstream.ignore(size);

            return std::make_shared<EAGLEye::TrackPathChunk>(EAGLEye::TrackPathChunk{id, size});
        }

        std::shared_ptr<EAGLEye::GeometryChunk> ParseGeometryChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            size_t bytesRead = 0;
            auto runTo = ((long) ifstream.tellg()) + size;

            GeometryFileInfo_s geometryFileInfo{};

            for (int i = 0; i < 0xFFFF && ifstream.tellg() < runTo; i++)
            {
                size_t scBytesRead = 0;

                uint32_t scId, scSize;
                bytesRead += readGeneric(ifstream, scId);
                bytesRead += readGeneric(ifstream, scSize);

                printf("Sub-chunk #%d: 0x%08x - %s\n", i + 1, scId, EAGLEye::chunkIdMap.find(scId)->second.c_str());

                // ifstream.ignore(scSize); bytesRead += scSize;
                switch (scId)
                {
                    case 0x80134001:
                    {
                        // parse file info
                        {
                            uint32_t bId, bSize;
                            scBytesRead += readGeneric(ifstream, bId);
                            assert(bId == 0x00134002);
                            scBytesRead += readGeneric(ifstream, bSize);
                            scBytesRead += readGeneric(ifstream, geometryFileInfo);
                        }

                        // sub-block 3
                        {
                            uint32_t bId, bSize;
                            scBytesRead += readGeneric(ifstream, bId);
                            assert(bId == 0x00134003);
                            scBytesRead += readGeneric(ifstream, bSize);
                            ifstream.ignore(bSize);
                            scBytesRead += bSize;
                        }

                        // null sub-block 8
                        {
                            uint32_t bId, bSize;
                            scBytesRead += readGeneric(ifstream, bId);
                            assert(bId == 0x80134008);
                            scBytesRead += readGeneric(ifstream, bSize);
                        }

                        bytesRead += scBytesRead;

//                        dumpBytes(ifstream, scSize - scBytesRead);

                        std::cout << geometryFileInfo.path << " [" << geometryFileInfo.section << "]" << std::endl;
                        ifstream.ignore(scSize - scBytesRead);
                        bytesRead += scSize - scBytesRead;
                        break;
                    }
                    case 0x80134010:
                    {
                        // sub-block 11
                        {
                            uint32_t bId, bSize;
                            scBytesRead += readGeneric(ifstream, bId);
                            assert(bId == 0x00134011);
                            scBytesRead += readGeneric(ifstream, bSize);
                        }

                        dumpBytes(ifstream, 128);
                        bytesRead += scBytesRead;
                        ifstream.ignore(scSize - scBytesRead);
                        bytesRead += scSize - scBytesRead;
                        break;
                    }
                    default:
                    {
                        ifstream.ignore(scSize);
                        bytesRead += scSize;
                        break;
                    }
                }
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

                printf("0x%08x - %s\n", id, EAGLEye::chunkIdMap.find(id)->second.c_str());

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
