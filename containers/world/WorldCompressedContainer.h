#ifndef EAGLEYE_WORLDCOMPRESSEDCONTAINER_H
#define EAGLEYE_WORLDCOMPRESSEDCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include <boost/filesystem.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        class WorldCompressedContainer : public Container<void>
        {
        public:
            explicit WorldCompressedContainer(std::ifstream &stream, boost::filesystem::path &filePath) : Container(
                    stream), m_filePath(filePath)
            {}

            void Get() override;

        private:
            boost::filesystem::path m_filePath;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_WORLDCOMPRESSEDCONTAINER_H
