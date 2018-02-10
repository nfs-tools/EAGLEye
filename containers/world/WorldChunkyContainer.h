#ifndef EAGLEYE_WORLDCHUNKYCONTAINER_H
#define EAGLEYE_WORLDCHUNKYCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        class WorldChunkyContainer : public Container<void>
        {
        public:
            explicit WorldChunkyContainer(std::ifstream &stream) : Container(stream)
            {}

            void Get() override;
        private:
            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}

#endif //EAGLEYE_WORLDCHUNKYCONTAINER_H
