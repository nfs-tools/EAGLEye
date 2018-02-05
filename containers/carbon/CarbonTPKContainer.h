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

        enum CarbonTPKChunks
        {

        };

        class CarbonTPKContainer : public Container<std::shared_ptr<TPK>>
        {
        public:
            CarbonTPKContainer(std::ifstream& stream, uint32_t containerSize);

            std::shared_ptr<TPK> ReadData() override;
        private:
            uint32_t m_containerSize;
            TPK* m_tpk;
        };
    }
}


#endif //EAGLEYE_CARBONTPKCONTAINER_H
