#ifndef EAGLEYE_UG2LOCATIONCONTAINER_H
#define EAGLEYE_UG2LOCATIONCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        class UG2LocationContainer : public Container<void>
        {
        public:
            explicit UG2LocationContainer(std::ifstream &stream, boost::filesystem::path &path) : Container(stream),
                                                                                                  m_path(path)
            {}

            void Get() override;

        private:
            size_t ReadChunks(uint32_t totalSize) override;

            boost::filesystem::path m_path;
        };
    }
}


#endif //EAGLEYE_UG2LOCATIONCONTAINER_H
