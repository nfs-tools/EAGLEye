#ifndef EAGLEYE_UNDERCOVERTPKCONTAINER_H
#define EAGLEYE_UNDERCOVERTPKCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../data/tpk/TexturePack.h"

namespace EAGLEye
{
    namespace Containers
    {
        using TPK = EAGLEye::Data::TexturePack;

        /**
         * Chunk IDs relating to Undercover's TPK format
         */
        enum UndercoverTPKChunks
        {
            UNDERCOVER_TPK = 0xb3310000,
            UNDERCOVER_TPK_PART_INFO = 0x33310001,
            UNDERCOVER_TPK_PART_HASHES = 0x33310002,
            UNDERCOVER_TPK_PART_HEADERS = 0x33310004,
            UNDERCOVER_TPK_PART_DXT_HEADERS = 0x33310005,

            UNDERCOVER_TPK_TEXTURE_DATA_ROOT = 0xb3320000,
            UNDERCOVER_TPK_TEXTURE_DATA_HASH = 0x33320001,
            UNDERCOVER_TPK_TEXTURE_DATA_CONTAINER = 0x33320002,
            UNDERCOVER_TPK_TEXTURE_DATA_CONTAINER_XENON = 0x33320300,
        };

        class UndercoverTPKContainer : public Container<std::shared_ptr<TPK>>
        {
        public:
            UndercoverTPKContainer(std::ifstream &stream, uint32_t containerSize);

            std::shared_ptr<TPK> Get() override;

        private:
            uint32_t m_containerSize;
            TPK *m_tpk;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_UNDERCOVERTPKCONTAINER_H
