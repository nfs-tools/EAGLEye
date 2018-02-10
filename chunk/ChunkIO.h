#ifndef EAGLEYE_CHUNKIO_H
#define EAGLEYE_CHUNKIO_H

#include "../eagldata.h"
#include "../eaglutils.h"
#include "../eaglenums.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

namespace EAGLEye
{
    namespace Chunks
    {
        typedef struct StreamInfoStruct
        {
            char ModelGroupName[8];
            unsigned int StreamChunkNumber;
            unsigned int Unk2;
            unsigned int MasterStreamChunkNumber;
            unsigned int MasterStreamChunkOffset;
            unsigned int Size1;
            unsigned int Size2;
            unsigned int Size3;
            unsigned int Unk3;
            float X;
            float Y;
            float Z;
            unsigned int StreamChunkHash;
            unsigned char RestOfData[0x24]; // 0x24 for MW, 0x1C for Carbon TODO: add game detection or something idk
        } *StreamInfo;

        void
        ExtractChunksToFolder(const boost::filesystem::path &filePath, const boost::filesystem::path &outFolderPath);

        void CombineFromFiles(const boost::filesystem::path &folderPath,
                              const boost::filesystem::path &fileName,
                              const boost::filesystem::path &outPath);

        /**
         * Stream functions
         */
        unsigned int SearchAlignedChunkByType(boost::filesystem::path& Filename, unsigned int ChunkMagic, long &OffsetOut);

        StreamInfo CreateStreamInfoBuffer(unsigned int InfoCount);

        unsigned int GetInfoCount(unsigned int InfoChunkSize);

        StreamInfo StreamInfoReader(unsigned int InfoCount, long InfoChunkAddress, boost::filesystem::path& LocBundlePath);

        bool ExtractAllStreamParts(StreamInfo TheStreamInfo, unsigned int InfoCount, boost::filesystem::path& StreamPath, boost::filesystem::path& OutFolderPath);

        bool ExtractStreamPartByNumber(unsigned int StreamPartNumber, StreamInfo TheStreamInfo, unsigned int InfoCount, boost::filesystem::path& StreamFilename, boost::filesystem::path& OutFolderPath);

        int CombineChunks(boost::filesystem::path& LocBundle, boost::filesystem::path& StreamChunkPaths, boost::filesystem::path& OutPath);
    }
}


#endif //EAGLEYE_CHUNKIO_H
