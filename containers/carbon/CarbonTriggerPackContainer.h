#ifndef EAGLEYE_CARBONTRIGGERPACKCONTAINER_H
#define EAGLEYE_CARBONTRIGGERPACKCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        class CarbonTriggerPackContainer : public Container<void>
        {
        public:
            CarbonTriggerPackContainer(std::ifstream &stream, uint32_t containerSize);

            void Get() override;

        private:
            uint32_t m_containerSize;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_CARBONTRIGGERPACKCONTAINER_H
