#ifndef EAGLEYE_UG2CHUNKYCONTAINER_H
#define EAGLEYE_UG2CHUNKYCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        struct UG2ReadOptions
        {
            long start = -1;
            long end = -1;
        };

        class UG2ChunkyContainer : public Container<void>
        {
        public:
            explicit UG2ChunkyContainer(std::ifstream &stream) : Container(stream)
            {}

            UG2ChunkyContainer(std::ifstream &stream, UG2ReadOptions options) : Container(stream), m_options(options)
            {}

            void Get() override;

        private:
            size_t ReadChunks(uint32_t totalSize) override;

            boost::filesystem::path m_path;
            UG2ReadOptions m_options;
        };
    }
}

#endif //EAGLEYE_UG2CHUNKYCONTAINER_H
