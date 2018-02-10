#include "ChunkIO.h"

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

                for (unsigned int i = 0; i <= ChunkSize - 1; i++)
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

        unsigned int GetInfoCount(unsigned int InfoChunkSize)
        {
            return InfoChunkSize / sizeof(StreamInfoStruct);
        }

        StreamInfo CreateStreamInfoBuffer(unsigned int InfoCount)
        {
            return (StreamInfo) malloc(sizeof(StreamInfoStruct) * InfoCount);
        }
    }
}