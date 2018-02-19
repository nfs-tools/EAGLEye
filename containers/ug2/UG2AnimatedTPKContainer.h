#ifndef EAGLEYE_UG2ANIMATEDTPKCONTAINER_H
#define EAGLEYE_UG2ANIMATEDTPKCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"
#include "../../data/tpk/TexturePack.h"

namespace EAGLEye
{
    namespace Containers
    {
        using TPK = EAGLEye::Data::TexturePack;

        /**
        * Chunk IDs relating to UG2's TPK format
        */
        enum UG2AnimatedTPKChunks
        {
            UG2_ANIMATED_TPK_HEADER = 0x30300101,
            UG2_ANIMATED_TPK_ENTRIES = 0x30300102,
            UG2_ANIMATED_TPK_HASHES = 0x30300103,
        };

        struct PACK UG2AnimatedTexture
        {
            char name[24];
            int hash;
            int numFrames;
            int fps;
            BYTE pad[16];
        };

        class UG2AnimatedTPKContainer : public Container<std::shared_ptr<TPK>>
        {
        public:
            UG2AnimatedTPKContainer(std::ifstream &stream, uint32_t containerSize);

            std::shared_ptr<TPK> Get() override;

        private:
            uint32_t m_containerSize;
            TPK *m_tpk;

            size_t ReadChunks(uint32_t totalSize) override;
            size_t ReadChunks(uint32_t totalSize, int depth);
        };
    }
}


#endif //EAGLEYE_UG2ANIMATEDTPKCONTAINER_H
