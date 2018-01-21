#ifndef EAGLEYE_CHUNKBASE_H
#define EAGLEYE_CHUNKBASE_H

#include "eaglutils.h"
#include "chunkstructures.h"

namespace EAGLEye
{
    class Chunk
    {
    public:
        Chunk(uint32_t id, uint32_t size);
    private:
        uint32_t id, size;
    };

    class EAGLAnimationsChunk : public Chunk
    {
    public:
        EAGLAnimationsChunk(uint32_t id, uint32_t size);

        std::vector<std::string> animationParts;
    };

    class TrackPathChunk : public Chunk
    {
    public:
        TrackPathChunk(uint32_t id, uint32_t size);
    };

    class GeometryChunk : public Chunk
    {
    public:
        GeometryChunk(uint32_t id, uint32_t size);
    };

    class TrackStreamerSectionsChunk : public Chunk
    {
    public:
        TrackStreamerSectionsChunk(uint32_t id, uint32_t size);

        std::vector<TrackStreamerSection_s> sections{};
    };

    class VisibleSectionChunk : public Chunk
    {
    public:
        VisibleSectionChunk(uint32_t id, uint32_t size);

        VisibleSectionStruct_s visibleSection;
    };
}

#endif //EAGLEYE_CHUNKBASE_H
