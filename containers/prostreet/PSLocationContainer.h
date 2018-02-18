#ifndef EAGLEYE_PSLOCATIONCONTAINER_H
#define EAGLEYE_PSLOCATIONCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        class PSLocationContainer : public Container<void>
        {
        public:
            explicit PSLocationContainer(std::ifstream &stream, boost::filesystem::path &path) : Container(stream),
                                                                                                 m_path(path)
            {}

            void Get() override;

        private:
            size_t ReadChunks(uint32_t totalSize) override;

            boost::filesystem::path m_path;
        };
    }
}


#endif //EAGLEYE_PSLOCATIONCONTAINER_H
