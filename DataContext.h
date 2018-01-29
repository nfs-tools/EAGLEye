#ifndef EAGLEYE_DATACONTEXT_H
#define EAGLEYE_DATACONTEXT_H

#include <utility>

#include "eaglutils.h"

namespace EAGLEye
{
    struct GeometryVertex
    {
        float x, y, z;
        float u, v;
        bool good;
    };

    struct GeometryFace
    {
        uint16_t vA;
        uint16_t vB;
        uint16_t vC;
    };

    struct GeometryMesh
    {
        std::vector<GeometryVertex> vertices;
        std::vector<GeometryFace> faces;
        std::vector<std::string> materialNames;
    };

    struct GeometryItem
    {
        std::string name;
        Point3D minPoint, maxPoint;
        long numTextures;
        GeometryMesh mesh;
        float identifier;
    };

    struct GeometryCatalog
    {
        GeometryCatalog(GeometryCatalog&&) = default;

        std::string filename;
        std::string section;
        std::vector<int32_t> hashTable;
        size_t numItems; // == hashTable.size()
        std::vector<std::unique_ptr<GeometryItem>> items;
        long alignBase;
    };

    class GlobalData
    {
    public:
        static std::vector<std::unique_ptr<GeometryCatalog>> catalogs;
    };
}


#endif //EAGLEYE_DATACONTEXT_H
