#ifndef EAGLEYE_MWLOCATIONCONTAINER_H
#define EAGLEYE_MWLOCATIONCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        class MWLocationContainer : public Container<void>
        {
        public:
            explicit MWLocationContainer(std::ifstream &stream, boost::filesystem::path &path) : Container(stream),
                                                                                                 m_path(path)
            {}

            void Get() override;

        private:
            size_t ReadChunks(uint32_t totalSize) override;

            boost::filesystem::path m_path;
        };
    }
}


#endif //EAGLEYE_MWLOCATIONCONTAINER_H
