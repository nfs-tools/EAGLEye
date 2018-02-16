#ifndef EAGLEYE_CARBONELIGHTCONTAINER_H
#define EAGLEYE_CARBONELIGHTCONTAINER_H

#include "../../Container.h"
#include "../../eaglenums.h"

namespace EAGLEye
{
    namespace Containers
    {
        enum CarbonLightChunkID
        {
            BCHUNK_CARBON_LIGHT_HEADER = 0x00135001,
            BCHUNK_CARBON_LIGHT_UNKNOWN = 0x00135002,
            BCHUNK_CARBON_LIGHT_ENTRIES = 0x00135003,
        };

        struct CarbonLightBINChunk
        {
            CarbonLightChunkID id;
            BYTE padding;
            const char *m_pszType;
        };

        class CarbonLightContainer : public Container<void>
        {
        public:
            CarbonLightContainer(std::ifstream &stream, uint32_t containerSize);

            void Get() override;

        private:
            uint32_t m_containerSize;

            size_t ReadChunks(uint32_t totalSize) override;

        protected:
            static CarbonLightBINChunk g_chunks[];
        };
    }
}


#endif //EAGLEYE_CARBONELIGHTCONTAINER_H
