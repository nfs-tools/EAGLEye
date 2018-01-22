#ifndef EAGLEYE_MW_H
#define EAGLEYE_MW_H

#include "../chunkbase.h"

#include <boost/smart_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/filesystem.hpp>

namespace EAGLEye
{
    namespace MW
    {
        std::shared_ptr<EAGLEye::TrackPathChunk> ParseTrackPathChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);
        std::shared_ptr<EAGLEye::GeometryChunk> ParseGeometryChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);
        std::shared_ptr<EAGLEye::EAGLAnimationsChunk> ParseAnimationsChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);
        std::shared_ptr<EAGLEye::TrackStreamerSectionsChunk> ParseTrackStreamerSectionsChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);
        std::shared_ptr<EAGLEye::VisibleSectionChunk> ParseVisibleSectionChunk(std::ifstream& ifstream, uint32_t id, uint32_t size);

        void HandleFile(boost::filesystem::path& path, std::ifstream& ifstream, EAGLEye::FileType fileType);
        void HandleLocationBaseFile(boost::filesystem::path &path, std::ifstream &ifstream);
        void HandleRegularFile(boost::filesystem::path &path, std::ifstream &ifstream);
        void HandleCompressedFile(boost::filesystem::path &path, std::ifstream &ifstream);
    }
}

#endif //EAGLEYE_MW_H
