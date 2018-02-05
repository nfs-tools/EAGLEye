#ifndef EAGLEYE_CARBONCHUNKEDCONTAINER_H
#define EAGLEYE_CARBONCHUNKEDCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        class CarbonChunkyContainer : public Container<void>
        {
        public:
            explicit CarbonChunkyContainer(std::ifstream &stream) : Container(stream)
            {}

            void ReadData() override;
        };
    }
}

#endif //EAGLEYE_CARBONCHUNKEDCONTAINER_H
