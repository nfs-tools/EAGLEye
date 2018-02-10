#ifndef EAGLEYE_WORLDVPAKCONTAINER_H
#define EAGLEYE_WORLDVPAKCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../data/tpk/TexturePack.h"

namespace EAGLEye
{
    namespace Containers
    {
        struct PACK MainHeader
        {
            int magic;
            int fileCount;
            int fileTableLocation;
            int fileTableLength;
        };

        struct PACK FileHeader
        {
            int fileNumber;
            int binLength;
            int vltLength;
            int binLocation;
            int vltLocation;
        };

        class WorldVPAKContainer : public Container<std::shared_ptr<void>>
        {
        public:
            explicit WorldVPAKContainer(std::ifstream &stream) : Container(stream)
            {}

            std::shared_ptr<void> Get() override;
        private:
            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_WORLDVPAKCONTAINER_H
