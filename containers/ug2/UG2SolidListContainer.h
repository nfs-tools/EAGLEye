#ifndef EAGLEYE_UG2GEOMETRYCONTAINER_H
#define EAGLEYE_UG2GEOMETRYCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        class UG2SolidListContainer : public Container<void>
        {
        public:
            UG2SolidListContainer(std::ifstream &stream, uint32_t containerSize);

            void Get() override;

        private:
            uint32_t m_containerSize;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_UG2GEOMETRYCONTAINER_H
