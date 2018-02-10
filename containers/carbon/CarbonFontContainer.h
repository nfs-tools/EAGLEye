#ifndef EAGLEYE_CARBONFONTCONTAINER_H
#define EAGLEYE_CARBONFONTCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        struct CarbonFont
        {
            // Blank for now
        };

        class CarbonFontContainer : public Container<std::shared_ptr<CarbonFont>>
        {
        public:
            CarbonFontContainer(std::ifstream &stream, uint32_t containerSize);

            std::shared_ptr<CarbonFont> Get() override;
        private:
            uint32_t m_containerSize;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}

#endif //EAGLEYE_CARBONFONTCONTAINER_H
