#ifndef EAGLEYE_SOLIDCATALOG_H
#define EAGLEYE_SOLIDCATALOG_H

#include "../../eaglutils.h"

namespace EAGLEye
{
    namespace Data
    {
        struct MeshVertex
        {
            float x, y, z, u, v;
        };

        struct MeshFace
        {
            short vA, vB, vC;
        };

        struct SolidItem
        {
            std::string name;
            std::vector<int> textureIds;
            std::vector<MeshVertex> vertices;
            std::vector<MeshFace> faces;
        };

        class SolidCatalog
        {
        public:
            SolidCatalog()
            = default;

            ~SolidCatalog()
            = default;

            std::string path;
            std::string section;
            std::vector<int32_t> hashTable;
            std::vector<SolidItem> items;
        };
    }
}


#endif //EAGLEYE_SOLIDCATALOG_H
