#ifndef EAGLEYE_PSCHUNKYCONTAINER_H
#define EAGLEYE_PSCHUNKYCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        struct PSReadOptions
        {
            long start = -1;
            long end = -1;
        };

        class PSChunkyContainer : public Container<void>
        {
        public:
            explicit PSChunkyContainer(std::ifstream &stream) : Container(stream), m_options({})
            {}

            PSChunkyContainer(std::ifstream &stream, PSReadOptions options) : Container(stream), m_options(options)
            {}

            void Get() override;

        private:
            size_t ReadChunks(uint32_t totalSize) override;

            PSReadOptions m_options;
        };
    }
}


#endif //EAGLEYE_PSCHUNKYCONTAINER_H
