#ifndef EAGLEYE_WORLD_H
#define EAGLEYE_WORLD_H

#include "../chunkbase.h"

#include <boost/smart_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/filesystem.hpp>

namespace EAGLEye
{
    namespace World
    {
        void HandleFile(boost::filesystem::path &path, std::ifstream &ifstream, EAGLEye::FileType fileType);

        void HandleLocationBaseFile(boost::filesystem::path &path, std::ifstream &ifstream);

        void HandleRegularFile(boost::filesystem::path &path, std::ifstream &ifstream);

        void HandleCompressedFile(boost::filesystem::path &path, std::ifstream &ifstream);
    }
}

#endif //EAGLEYE_WORLD_H
