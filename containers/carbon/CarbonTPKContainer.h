#ifndef EAGLEYE_CARBONTPKCONTAINER_H
#define EAGLEYE_CARBONTPKCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../data/tpk/TexturePack.h"

namespace EAGLEye
{
    namespace Containers
    {
        using TPK = EAGLEye::Data::TexturePack;

        /**
         * Chunk IDs relating to Carbon's TPK format
         */
        enum CarbonTPKChunks
        {
            CARBON_TPK = 0xb3310000,
            CARBON_TPK_TEXTURE_DATA_ROOT = 0xb3320000,
            CARBON_TPK_TEXTURE_DATA_HASH = 0x33320001,
            CARBON_TPK_TEXTURE_DATA_CONTAINER = 0x33320002,
            CARBON_TPK_TEXTURE_DATA_CONTAINER_XENON = 0x33320300,

            CARBON_TPK_PART_INFO = 0x33310001,
            CARBON_TPK_PART_HASHES = 0x33310002,
            CARBON_TPK_PART_DYNAMIC_DATA = 0x33310003,
            CARBON_TPK_PART_HEADERS = 0x33310004,
            CARBON_TPK_PART_DXT_HEADERS = 0x3331005,
            CARBON_TPK_ANIMATED_TEXTURES = 0xb3312000,
            CARBON_TPK_ANIMATED_TEXTURES_ROOT = 0xb3312004,
            CARBON_TPK_ANIMATED_TEXTURES_ENTRY = 0x33312001,
            CARBON_TPK_ANIMATED_TEXTURES_HASHES = 0x33312002,
        };

        class CarbonTPKContainer : public Container<std::shared_ptr<TPK>>
        {
        public:
            CarbonTPKContainer(std::ifstream &stream, uint32_t containerSize);

            std::shared_ptr<TPK> Get() override;

        private:
            uint32_t m_containerSize;
            TPK *m_tpk;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_CARBONTPKCONTAINER_H
