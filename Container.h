#ifndef EAGLEYE_CONTAINER_H
#define EAGLEYE_CONTAINER_H

#include "eaglutils.h"

#include <boost/filesystem/path.hpp>

namespace EAGLEye
{
    namespace Containers
    {
        /**
         * A container is a generic class for reading chunk data.
         * For example, a compressed file (*.lzc) can be handled through a container.
         * For another example, the code to read a geometry chunk might be placed
         * into its own container.
         */
        template<typename T = void>
        class Container
        {
        public:
            explicit Container(std::ifstream &stream) : m_stream(stream)
            {
            }

            virtual T Get()= 0;

            virtual ~Container() = default;

            Container(const Container &c) = delete;

            Container(const Container &&c) = delete;

            Container &operator=(const Container &c) = delete;

            Container &operator=(const Container &&c) = delete;

        private:
            virtual size_t ReadChunks(uint32_t totalSize) = 0;

        protected:
            std::ifstream &m_stream;
        };
    }
}

#endif //EAGLEYE_CONTAINER_H
