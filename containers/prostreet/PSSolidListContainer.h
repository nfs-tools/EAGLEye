#ifndef EAGLEYE_PSSOLIDLISTCONTAINER_H
#define EAGLEYE_PSSOLIDLISTCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../eagltypes.h"

namespace EAGLEye
{
    namespace Containers
    {
        class PSSolidListContainer: public Container<void>
        {
        public:
            PSSolidListContainer(std::ifstream &stream, uint32_t containerSize);

            void Get() override;

        private:
            uint32_t m_containerSize;

            size_t ReadChunks(uint32_t totalSize) override;

            size_t ReadChunks(uint32_t totalSize, int depth);
        };
    }
}


#endif //EAGLEYE_PSSOLIDLISTCONTAINER_H
