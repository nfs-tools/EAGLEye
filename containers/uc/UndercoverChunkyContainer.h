#ifndef EAGLEYE_UNDERCOVERCHUNKYCONTAINER_H
#define EAGLEYE_UNDERCOVERCHUNKYCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        struct UCReadOptions
        {
            long start = -1;
            long end = -1;
        };

        class UndercoverChunkyContainer : public Container<void>
        {
        public:
            explicit UndercoverChunkyContainer(std::ifstream &stream) : Container(stream)
            {}

            UndercoverChunkyContainer(std::ifstream &stream, UCReadOptions options) : Container(stream), m_options(options)
            {}

            void Get() override;

        private:
            size_t ReadChunks(uint32_t totalSize) override;

            UCReadOptions m_options;
        };
    }
}


#endif //EAGLEYE_UNDERCOVERCHUNKYCONTAINER_H
