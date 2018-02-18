#ifndef EAGLEYE_PSTPKCONTAINER_H
#define EAGLEYE_PSTPKCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../data/tpk/TexturePack.h"

namespace EAGLEye
{
    namespace Containers
    {
        using TPK = EAGLEye::Data::TexturePack;

        /**
         * Chunk IDs relating to ProStreet's TPK format
         */
        enum PSTPKChunks
        {
            PS_TPK = 0xb3310000,
            PS_TPK_PART_INFO = 0x33310001,
            PS_TPK_PART_HASHES = 0x33310002,
            PS_TPK_PART_HEADERS = 0x33310004,
            PS_TPK_PART_DXT_HEADERS = 0x33310005,

            PS_TPK_TEXTURE_DATA_ROOT = 0xb3320000,
            PS_TPK_TEXTURE_DATA_HASH = 0x33320001,
            PS_TPK_TEXTURE_DATA_CONTAINER = 0x33320002,
            PS_TPK_TEXTURE_DATA_CONTAINER_XENON = 0x33320300,
        };

        struct PACK PSTPKHeader
        {
            BYTE marker[4];
            char name[0x1C];
            char path[0x40];
            int hash;
            BYTE pad[24];
        };

        class PSTPKContainer : public Container<std::shared_ptr<TPK>>
        {
        public:
            PSTPKContainer(std::ifstream &stream, uint32_t containerSize);

            std::shared_ptr<TPK> Get() override;

        private:
            uint32_t m_containerSize;
            TPK *m_tpk;

            size_t ReadChunks(uint32_t totalSize) override;
        };
    }
}


#endif //EAGLEYE_PSTPKCONTAINER_H
