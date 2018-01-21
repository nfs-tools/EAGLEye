#include "chunkbase.h"

namespace EAGLEye
{
    Chunk::Chunk(uint32_t id, uint32_t size) : id(id), size(size)
    {

    }

    TrackPathChunk::TrackPathChunk(uint32_t id, uint32_t size) : Chunk(id, size)
    {

    }

    GeometryChunk::GeometryChunk(uint32_t id, uint32_t size) : Chunk(id, size)
    {

    }

    EAGLAnimationsChunk::EAGLAnimationsChunk(uint32_t id, uint32_t size) : Chunk(id, size)
    {

    }

    TrackStreamerSectionsChunk::TrackStreamerSectionsChunk(uint32_t id, uint32_t size) : Chunk(id, size)
    {

    }

    VisibleSectionChunk::VisibleSectionChunk(uint32_t id, uint32_t size) : Chunk(id, size)
    {

    }
}