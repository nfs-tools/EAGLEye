#ifndef EAGLEYE_CHUNKIO_H
#define EAGLEYE_CHUNKIO_H

#include "../eagldata.h"
#include "../eaglutils.h"
#include "../eaglenums.h"

#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#define STREAMINFOCHUNK 0x00034110

namespace EAGLEye
{
    namespace Chunks
    {
        typedef struct PACK StreamInfoStruct
        {
            char ModelGroupName[8]; // 8
            unsigned int StreamChunkNumber; // 12
            unsigned int Unk2; // 16
            unsigned int MasterStreamChunkNumber; // 20
            unsigned int MasterStreamChunkOffset; // 24
            unsigned int Size1; // 28
            unsigned int Size2; // 32
            unsigned int Size3; // 36
            unsigned int Unk3; // 40
            float X; // 44
            float Y; // 48
            float Z; // 52
            unsigned int StreamChunkHash; // 56

            // 0x24 for MW, 0x1C for Carbon TODO: add game detection or something idk
            unsigned char RestOfData[0x24]; // 92
        } *StreamInfo;

        void
        ExtractChunksToFolder(const boost::filesystem::path &filePath, const boost::filesystem::path &outFolderPath);

        void CombineFromFiles(const boost::filesystem::path &folderPath,
                              const boost::filesystem::path &fileName,
                              const boost::filesystem::path &outPath);

        /**
         * Stream functions
         */
        unsigned int
        SearchAlignedChunkByType(boost::filesystem::path &Filename, unsigned int ChunkMagic, long &OffsetOut);

        StreamInfo CreateStreamInfoBuffer(unsigned int InfoCount);

        unsigned int GetInfoCount(unsigned int InfoChunkSize);

        StreamInfo
        StreamInfoReader(unsigned int InfoCount, long InfoChunkAddress, boost::filesystem::path &LocBundlePath);

        bool
        ExtractAllStreamParts(StreamInfo TheStreamInfo, unsigned int InfoCount, boost::filesystem::path &StreamPath,
                              boost::filesystem::path &OutFolderPath);

        bool ExtractStreamPartByNumber(unsigned int StreamPartNumber, StreamInfo TheStreamInfo, unsigned int InfoCount,
                                       boost::filesystem::path &StreamFilename, boost::filesystem::path &OutFolderPath);

        int CombineStreamChunks(boost::filesystem::path &LocBundle, boost::filesystem::path &StreamChunkPaths,
                                boost::filesystem::path &OutPath);

        int ExtractStreamChunks(boost::filesystem::path &LocBundle,
                                boost::filesystem::path &OutPath);
    }
}


#endif //EAGLEYE_CHUNKIO_H
