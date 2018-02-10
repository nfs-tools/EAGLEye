#ifndef EAGLEYE_CARBONSCENERYCONTAINER_H
#define EAGLEYE_CARBONSCENERYCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        struct PACK SceneryStruct
        {
            char name[24];
            BYTE data[48];
        };

        using Scenery = std::vector<SceneryStruct>;

        class CarbonSceneryContainer : public Container<Scenery>
        {
        public:
            CarbonSceneryContainer(std::ifstream &stream, uint32_t containerSize);

            Scenery Get() override;

        private:
            uint32_t m_containerSize;
            Scenery m_scenery;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}

#endif //EAGLEYE_CARBONSCENERYCONTAINER_H
