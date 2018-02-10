#ifndef EAGLEYE_WORLDTPKCONTAINER_H
#define EAGLEYE_WORLDTPKCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../data/tpk/TexturePack.h"
#include "WorldStructs.h"

namespace EAGLEye
{
    namespace Containers
    {
        /**
         * Chunk IDs relating to Carbon's TPK format
         */
        enum WorldTPKChunks
        {
            WORLD_TPK = 0xb3310000,
            WORLD_TPK_TEXTURE_DATA = 0xb3320000,

            WORLD_TPK_PART_INFO = 0x33310001,
            WORLD_TPK_PART_HASHES = 0x33310002,
            WORLD_TPK_PART_DYNAMIC_DATA = 0x33310003,
            WORLD_TPK_PART_HEADERS = 0x33310004,
            WORLD_TPK_PART_DXT_HEADERS = 0x3331005,
        };

        using TPK = EAGLEye::Data::TexturePack;

        class WorldTPKContainer : public Container<std::shared_ptr<TPK>>
        {
        public:
            WorldTPKContainer(std::ifstream &stream, uint32_t containerSize);

            std::shared_ptr<TPK> Get() override;
        private:
            uint32_t m_containerSize;
            TPK *m_tpk;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_WORLDTPKCONTAINER_H
