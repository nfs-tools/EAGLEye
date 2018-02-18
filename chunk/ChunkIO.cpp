#include "ChunkIO.h"

#define PRINTTYPEINFO "INFO:"
#define PRINTTYPEERROR "ERROR:"
#define PRINTTYPEWARNING "WARNING:"

#include <boost/format.hpp>

namespace EAGLEye
{
    namespace Chunks
    {
        int WriteChunkTypeAndSize(std::ofstream &ofstream, unsigned int ChunkMagic, unsigned int ChunkSize)
        {
            EAGLEye::writeGeneric<unsigned int>(ofstream, ChunkMagic);
            EAGLEye::writeGeneric<unsigned int>(ofstream, ChunkSize);
            return 1;
        }

        int ZeroChunkWriter(std::ofstream &ofstream, unsigned int ChunkSize)
        {
            if (ChunkSize)
            {
                WriteChunkTypeAndSize(ofstream, 0, ChunkSize);
                printf("%s Zero chunk: writing %04X (%d) bytes\n", "INFO:", ChunkSize, ChunkSize);

                for (unsigned int i = 0; i < ChunkSize; i++)
                    EAGLEye::writeGeneric<BYTE>(ofstream, 0x00);
            }
            return 1;
        }

        void
        ExtractChunksToFolder(const boost::filesystem::path &filePath, const boost::filesystem::path &outFolderPath)
        {
            assert(boost::filesystem::is_regular_file(filePath));

            if (!boost::filesystem::exists(outFolderPath))
            {
                assert(boost::filesystem::create_directories(outFolderPath));
            }

            assert(boost::filesystem::is_directory(outFolderPath));

            std::ifstream stream(filePath.string(), std::ios::binary);

            auto runTo = getFileLength(stream);

            char *partBuffer;

            for (int i = 0; i < 0xFFFF && stream.tellg() < runTo; i++)
            {
                uint32_t id, size;
                readGeneric(stream, id);
                readGeneric(stream, size);

                stream.seekg(-8, stream.cur);

                partBuffer = (char *) malloc(size + 8);

                stream.read(partBuffer, size + 8);

                boost::filesystem::path partPath(outFolderPath);
                partPath.append(filePath.filename().stem().string());
                partPath += std::string("-part") + std::to_string(i + 1);
                partPath += std::string(".BUN");

                std::cout << partPath.string() << std::endl;

                std::ofstream partFile(partPath.string(), std::ios::binary | std::ios::trunc);
                partFile.write((const char *) partBuffer, size + 8);

                free(partBuffer);
            }
        }

        void CombineFromFiles(const boost::filesystem::path &folderPath,
                              const boost::filesystem::path &fileName,
                              const boost::filesystem::path &outPath)
        {
            assert(outPath.has_filename());

            boost::regex regex(fileName.stem().string() + "-part([0-9]+)\\.BUN");

            std::ofstream ofstream(outPath.string(), std::ios::binary | std::ios::trunc);

            std::vector<boost::filesystem::path> paths;

            boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
            for (boost::filesystem::directory_iterator i(folderPath); i != end_itr; ++i)
            {
                const auto &path = i->path();

                if (boost::regex_match(path.filename().string(), regex))
                {
                    paths.emplace_back(path);
                }
            }

            char *chunkBuffer;

            unsigned int chunkAlignSize;

            for (int i = 0; i < paths.size(); i++)
            {
                const auto &path = paths[i];
                uintmax_t fileSize = boost::filesystem::file_size(path);
                std::ifstream fileStream(path.string(), std::ios::binary);

                printf("%s -> %ju\n", path.filename().string().c_str(), fileSize);

                chunkBuffer = (char *) malloc(fileSize);

                fileStream.read(chunkBuffer, fileSize);

                ofstream.write((const char *) chunkBuffer, fileSize);

                free(chunkBuffer);

                if (i != paths.size() - 1)
                {
                    chunkAlignSize =
                            (unsigned int) (((fileSize + 8)) - ((fileSize + 8) % 0x800)) +
                            0x1000;
                    chunkAlignSize -= fileSize + 8;

                    ZeroChunkWriter(ofstream, chunkAlignSize);
                }
            }
        }

        unsigned int SearchAlignedChunkByType(std::ifstream &stream, unsigned int ChunkMagic, long &OffsetOut)
        {
            unsigned int ReadMagic = 0;
            unsigned int ReadSize = 0;

            while (!stream.eof())
            {
                readGeneric(stream, ReadMagic);
                readGeneric(stream, ReadSize);

                if (ReadSize && (ReadMagic == ChunkMagic))
                {
                    OffsetOut = ((long) stream.tellg()) - 8;
                    return ReadSize;
                }
                if (ReadSize)
                    stream.seekg(ReadSize, stream.cur);
            }

            return ReadSize;
        }

        unsigned int
        SearchAlignedChunkByType(boost::filesystem::path &Filename, unsigned int ChunkMagic, long &OffsetOut)
        {
            std::ifstream stream(Filename.string(), std::ios::binary);

            if (stream.bad())
            {
                printf("%s Can't open file: %s\n", "ERROR:", Filename.c_str());
                perror("ERROR");
                return 0;
            }

            return SearchAlignedChunkByType(stream, ChunkMagic, OffsetOut);
        }

        unsigned int GetInfoCount(unsigned int InfoChunkSize)
        {
            return InfoChunkSize / sizeof(StreamInfoStruct);
        }

        StreamInfo
        StreamInfoReader(unsigned int InfoCount, long InfoChunkAddress, boost::filesystem::path &LocBundlePath)
        {
            char LocationName[255];
            boost::filesystem::path BundleName = LocBundlePath.filename();

            StreamInfo TheStreamInfo = CreateStreamInfoBuffer(InfoCount);

            std::ifstream stream(LocBundlePath.string(), std::ios::binary);

            if (stream.fail())
            {
                printf("%s Can't open file: %s\n", PRINTTYPEERROR, LocBundlePath.c_str());
                perror("ERROR");
                return 0;
            }

//            strncpy(dst, src, size);

            strncpy(LocationName, BundleName.stem().c_str(), BundleName.stem().string().length());
            LocationName[strrchr(BundleName.c_str(), '.') - BundleName.c_str()] = 0;

            stream.seekg(InfoChunkAddress + 8);

            for (unsigned int i = 0; i < InfoCount; i++)
            {
                stream.read((char *) &TheStreamInfo[i], sizeof(StreamInfoStruct));
            }

            return TheStreamInfo;
        }

        bool
        ExtractAllStreamParts(StreamInfo TheStreamInfo, unsigned int InfoCount, boost::filesystem::path &StreamPath,
                              boost::filesystem::path &OutFolderPath)
        {
            char *partBuffer;
            boost::filesystem::path streamFilename = StreamPath.filename().stem();

            char *StreamPartName = (char *) malloc(streamFilename.string().length() + 5);
            char *OutFilename = (char *) malloc(OutFolderPath.string().length() + streamFilename.string().length() + 5);

            printf("%s Opening file: %s\n", PRINTTYPEINFO, StreamPath.c_str());

            std::ifstream stream(StreamPath.string(), std::ios::binary);

            if (stream.fail())
            {
                printf("%s Can't open file: %s\n", PRINTTYPEERROR, StreamPath.c_str());
                perror("ERROR");
                return 0;
            }

            for (unsigned int i = 0; i < InfoCount; i++)
            {
                sprintf(StreamPartName, "%s_%d", streamFilename.c_str(), TheStreamInfo[i].StreamChunkNumber);
                sprintf(OutFilename, "%s/%s.BUN", OutFolderPath.c_str(), StreamPartName);

                std::ofstream partStream(OutFilename, std::ios::binary | std::ios::trunc);

                if (partStream.fail())
                {
                    printf("%s Can't open file: %s\n", PRINTTYPEERROR, OutFilename);
                    perror("ERROR");
                    return 0;
                }

                printf("%s Writing: %s\n", PRINTTYPEINFO, OutFilename);
                partBuffer = (char *) malloc(TheStreamInfo[i].Size1);

                stream.seekg(TheStreamInfo[i].MasterStreamChunkOffset);
                stream.read(partBuffer, TheStreamInfo[i].Size1);

                partStream.write(partBuffer, TheStreamInfo[i].Size1);

                free(partBuffer);
            }

            return 1;
        }

        bool ExtractStreamPartByNumber(unsigned int StreamPartNumber, StreamInfo TheStreamInfo, unsigned int InfoCount,
                                       boost::filesystem::path &StreamFilename, boost::filesystem::path &OutFolderPath)
        {
            return true;
        }

        int CombineStreamChunks(boost::filesystem::path &LocBundle, boost::filesystem::path &StreamChunkPaths,
                                boost::filesystem::path &OutPath)
        {
            std::string BundleName;
            std::string LocationName;
            std::string StreamName;

            std::stringstream TotalStreamChunkPath;
            boost::filesystem::path TotalOutPath;

            unsigned int InfoCount = 0;
            unsigned int SizeOfInfoChunk = 0;
            long InfoChunkOffset = 0;

            char *chunkBuffer;
            StreamInfo CombinerStreamInfo;

            printf("%s Opening %s\n", PRINTTYPEINFO, LocBundle.c_str());
            std::ifstream LocBundleIn(LocBundle.string(), std::ios::binary);

            if (LocBundleIn.fail())
            {
                printf("%s Couldn't open file: %s\n", PRINTTYPEERROR, LocBundle.c_str());
                perror("ERROR");
                return -1;
            }

            BundleName = LocBundle.filename().string();
            LocationName = LocBundle.filename().stem().string();

            printf("%s Bundle name: %s\n", PRINTTYPEINFO, BundleName.c_str());
            printf("%s Location name: %s\n", PRINTTYPEINFO, LocationName.c_str());

            TotalOutPath = OutPath / LocBundle.filename();

            uintmax_t fileSize = boost::filesystem::file_size(LocBundle);

            chunkBuffer = (char *) malloc(fileSize);

            LocBundleIn.read(chunkBuffer, fileSize);

            printf("%s Opening %s for writing\n", PRINTTYPEINFO, TotalOutPath.c_str());
            std::ofstream LocBundleOut(TotalOutPath.c_str(), std::ios::binary | std::ios::trunc);

            if (LocBundleOut.fail())
            {
                printf("%s Couldn't open file for writing: %s\n", PRINTTYPEERROR, TotalOutPath.c_str());
                perror("ERROR");
                return -1;
            }

            LocBundleOut.write(chunkBuffer, fileSize);

            printf("%s Wrote %lu bytes\n", PRINTTYPEINFO, fileSize);

            SizeOfInfoChunk = SearchAlignedChunkByType(LocBundle, STREAMINFOCHUNK, InfoChunkOffset);
            printf("%s Found at %lX, size %X\n", PRINTTYPEINFO, InfoChunkOffset, SizeOfInfoChunk);
            InfoCount = GetInfoCount(SizeOfInfoChunk);
            printf("Total info count = %d\n", InfoCount);
            //CombinerStreamInfo = CreateStreamInfoBuffer(InfoCount);
            CombinerStreamInfo = StreamInfoReader(InfoCount, InfoChunkOffset, LocBundle);

            TotalOutPath.clear();
            TotalOutPath = OutPath / boost::filesystem::path(std::string("STREAM") + BundleName);

            printf("%s Output path: %s\n", PRINTTYPEINFO, TotalOutPath.c_str());

//            {
//                unsigned int ChunkAlignSize;
//
//                std::ofstream StreamFileOut(TotalOutPath.string(), std::ios::binary | std::ios::trunc);
//
//                if (StreamFileOut.fail())
//                {
//                    printf("%s Can't open file: %s\n", PRINTTYPEERROR, TotalOutPath.c_str());
//                    perror("ERROR");
//                    return -1;
//                }
//
//                for (unsigned int i = 0; i < InfoCount; i++)
//                {
//                    // set the offset and chunk number
//                    CombinerStreamInfo[i].MasterStreamChunkNumber = i + 1;
//                    CombinerStreamInfo[i].MasterStreamChunkOffset = (unsigned int) StreamFileOut.tellp();
//
//                    printf("%s Offset: %lX\n", PRINTTYPEINFO, (long) StreamFileOut.tellp());
//
//                    StreamName = (boost::format("STREAM%1%_%2%.BUN") % LocationName %
//                                  CombinerStreamInfo[i].StreamChunkNumber).str();
////                    sprintf(StreamName, "STREAM%s_%d.BUN", LocationName, CombinerStreamInfo[i].StreamChunkNumber);
//                    printf("%s Reading: %s\n", PRINTTYPEINFO, StreamName.c_str());
//
//                    TotalStreamChunkPath << StreamChunkPaths.string();
//                    TotalStreamChunkPath << StreamChunkPaths.preferred_separator;
//                    TotalStreamChunkPath << StreamName;
//
////                    strcpy(TotalStreamChunkPath, StreamChunkPaths.c_str());
////                    strcat(TotalStreamChunkPath, "/");
////                    strcat(TotalStreamChunkPath, StreamName);
//
//                    printf("%s TotalStreamChunkPath: %s\n", PRINTTYPEINFO, TotalStreamChunkPath.str().c_str());
//
//                    std::ifstream partStream(TotalStreamChunkPath.str(), std::ios::binary);
//
//                    if (partStream.fail())
//                    {
//                        printf("%s Can't open file: %s\n", PRINTTYPEERROR, TotalStreamChunkPath.str().c_str());
//                        perror("ERROR");
//                        return -1;
//                    }
//
//                    uintmax_t partSize = boost::filesystem::file_size(
//                            boost::filesystem::path(TotalStreamChunkPath.str()));
//
//                    printf("%s Chunk size: %lX; current size: %X\n", PRINTTYPEINFO, partSize, CombinerStreamInfo[i].Size1);
//
//                    // size3 unknown, so don't touch unless it's identical
//                    CombinerStreamInfo[i].Size3 = (unsigned int) partSize;
//
//                    CombinerStreamInfo[i].Size1 = (unsigned int) partSize;
//                    CombinerStreamInfo[i].Size2 = (unsigned int) partSize;
//
//                    // allocate memory, copy to master stream, free memory, close input file
//                    chunkBuffer = (char *) malloc(CombinerStreamInfo[i].Size1);
//
//                    partStream.read(chunkBuffer, CombinerStreamInfo[i].Size1);
//                    StreamFileOut.write(chunkBuffer, CombinerStreamInfo[i].Size1);
//
//                    free(chunkBuffer);
//
//                    if (i != (InfoCount - 1))
//                    {
//                        // calculate and write alignment chunk, align by 0x800 bytes
//                        ChunkAlignSize =
//                                (((CombinerStreamInfo[i].Size1 + 8)) - ((CombinerStreamInfo[i].Size1 + 8) % 0x800)) +
//                                0x1000;
//
//                        ChunkAlignSize -= CombinerStreamInfo[i].Size1 + 8;
//
//                        ZeroChunkWriter(StreamFileOut, ChunkAlignSize);
//                    }
//
//                    TotalStreamChunkPath.str("");
//                }
//            }

            InfoChunkOffset += 8;
            LocBundleOut.seekp(InfoChunkOffset);

            {
                size_t bytesWritten = 0;

                for (unsigned int i = 0; i < InfoCount; i++)
                    bytesWritten += writeGeneric(LocBundleOut, CombinerStreamInfo[i]);

                printf("%s Bytes written to LocBundle: %zu\n", PRINTTYPEINFO, bytesWritten);
            }

//            std::string BundleName = LocBundle.filename().string();

            return -1;
        }

        // Extracts stream parts
        // LocBundle - The original location bundle path (e.g. L5RA.BUN)
        // OutPath - Location where the stream parts will go
        int ExtractStreamChunks(boost::filesystem::path &LocBundle,
                                boost::filesystem::path &OutPath)
        {
            const char *BundleName = LocBundle.filename().c_str();

            char *InputPath;

            char *FullStreamPath;
            char StreamName[255];
            char LocationName[255];

            unsigned int SizeOfInfoChunk = 0;
            unsigned int InfoCount = 0;
            long InfoChunkOffset = 0;

            StreamInfo ExtractorStreamInfo;

            printf("%s Opening %s\n", PRINTTYPEINFO, LocBundle.c_str());

            std::ifstream stream(LocBundle.string(), std::ios::binary);

            if (stream.fail())
            {
                printf("%s Can't open file: %s\n", PRINTTYPEERROR, LocBundle.c_str());
                perror("ERROR");
                return -1;
            }

            printf("%s Bundle name: %s\n", PRINTTYPEINFO, BundleName);
            strncpy(LocationName, BundleName, strrchr(BundleName, '.') + 1 - BundleName);
            LocationName[strrchr(BundleName, '.') - BundleName] = 0;
            printf("%s Location name: %s\n", PRINTTYPEINFO, LocationName);

            SizeOfInfoChunk = SearchAlignedChunkByType(LocBundle, STREAMINFOCHUNK, InfoChunkOffset);
            printf("%s Found at %lX, size %X\n", PRINTTYPEINFO, InfoChunkOffset, SizeOfInfoChunk);
            InfoCount = GetInfoCount(SizeOfInfoChunk);
            printf("Total info count = %d\n", InfoCount);

            ExtractorStreamInfo = StreamInfoReader(InfoCount, InfoChunkOffset, LocBundle);

            sprintf(StreamName, "STREAM%s.BUN", LocationName);
            boost::filesystem::path fullPath = LocBundle.parent_path() / boost::filesystem::path(StreamName);

            printf("%s Extracting stream chunks to %s\n", PRINTTYPEINFO, OutPath.c_str());
            printf("%s StreamName: %s\n", PRINTTYPEINFO, StreamName);
            printf("%s FullPath:   %s\n", PRINTTYPEINFO, fullPath.c_str());

            if (ExtractAllStreamParts(ExtractorStreamInfo, InfoCount, fullPath, OutPath))
                printf("%s Extraction finished successfuly!\n", PRINTTYPEINFO);
            else
                printf("%s Extraction finished with errors.\n", PRINTTYPEWARNING);

            return 0;
        }

        StreamInfo CreateStreamInfoBuffer(unsigned int InfoCount)
        {
            return (StreamInfo) malloc(sizeof(StreamInfoStruct) * InfoCount);
        }
    }
}