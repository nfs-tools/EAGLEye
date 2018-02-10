#ifndef EAGLEYE_CARBONSECTIONSCONTAINER_H
#define EAGLEYE_CARBONSECTIONSCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../eagldata.h"

namespace EAGLEye
{
    namespace Containers
    {
        using Sections = std::vector<StreamInfoStruct>;

        class CarbonSectionsContainer : public Container<Sections>
        {
        public:
            CarbonSectionsContainer(std::ifstream &stream, uint32_t containerSize);

            Sections Get() override;
        private:
            uint32_t m_containerSize;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_CARBONSECTIONSCONTAINER_H
