#ifndef EAGLEYE_CARBONCHUNKEDCONTAINER_H
#define EAGLEYE_CARBONCHUNKEDCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "CarbonAnimationManager.h"

namespace EAGLEye
{
    namespace Containers
    {
        class CarbonChunkyContainer : public Container<void>
        {
        public:
            explicit CarbonChunkyContainer(std::ifstream &stream, boost::filesystem::path &path) : Container(stream),
                                                                                                   m_path(path),
                                                                                                   m_animationManager(nullptr)
            {}

            void Get() override;

        private:
            size_t ReadChunks(uint32_t totalSize) override;

            boost::filesystem::path m_path;
            CarbonAnimationManager* m_animationManager;
        };
    }
}

#endif //EAGLEYE_CARBONCHUNKEDCONTAINER_H
