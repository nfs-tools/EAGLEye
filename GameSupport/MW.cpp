#include "MW.h"
#include <iostream>

namespace EAGLEye
{
    namespace MW
    {
        std::shared_ptr<EAGLEye::TrackPathChunk>
        ParseTrackPathChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            ifstream.ignore(size);

            return std::make_shared<EAGLEye::TrackPathChunk>(EAGLEye::TrackPathChunk{id, size});
        }

        std::shared_ptr<EAGLEye::GeometryChunk> ParseGeometryChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            return std::make_shared<EAGLEye::GeometryChunk>(EAGLEye::GeometryChunk{id, size});
        }

        std::shared_ptr<EAGLEye::EAGLAnimationsChunk>
        ParseAnimationsChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            EAGLEye::EAGLAnimationsChunk chunk{id, size};
            size_t bytesRead = 0;
            AnimationsStruct_s animationsStruct_s{};

            bytesRead += readGeneric(ifstream, animationsStruct_s);
            ifstream.ignore(2);
            bytesRead += 2;

//            dumpBytes(ifstream, 256);
            ifstream.ignore(size - bytesRead);

            return std::make_shared<EAGLEye::EAGLAnimationsChunk>(chunk);
        }

        std::shared_ptr<EAGLEye::TrackStreamerSectionsChunk>
        ParseTrackStreamerSectionsChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            EAGLEye::TrackStreamerSectionsChunk chunk{id, size};

            for (int i = 0; i < size / sizeof(EAGLEye::TrackStreamerSection_s); i++)
            {
                EAGLEye::TrackStreamerSection_s section{};
                readGeneric(ifstream, section);
                chunk.sections.emplace_back(section);
            }

            return std::make_shared<EAGLEye::TrackStreamerSectionsChunk>(chunk);
        }

        std::shared_ptr<EAGLEye::VisibleSectionChunk>
        ParseVisibleSectionChunk(std::ifstream &ifstream, uint32_t id, uint32_t size)
        {
            VisibleSectionStruct_s visibleSectionStruct{};
            readGeneric(ifstream, visibleSectionStruct);

            ifstream.ignore(size - sizeof(visibleSectionStruct));

            EAGLEye::VisibleSectionChunk chunk{id, size};

            chunk.visibleSection = visibleSectionStruct;

            std::cout << "Visible Section: " << visibleSectionStruct.name << std::endl;

            return std::make_shared<EAGLEye::VisibleSectionChunk>(chunk);
        }

        void HandleFile(boost::filesystem::path &path, std::ifstream &ifstream, EAGLEye::FileType fileType)
        {
            switch (fileType)
            {
                case EAGLEye::LANGUAGE:
                    HandleLanguageFile(ifstream);
                    break;
                case EAGLEye::LOCATION_FULL:
                    HandleLocationFullFile(ifstream);
                    break;
                case EAGLEye::LOCATION_BASE:
                    HandleLocationBaseFile(ifstream);
                    break;
                case EAGLEye::GLOBAL:
                    HandleGlobalFile(ifstream);
                case EAGLEye::GENERIC_CHUNKED:
                default:
                    HandleGenericChunkedFile(ifstream);
            }
        }

        void HandleLanguageFile(std::ifstream &ifstream)
        {
            std::cout << "Language file!" << std::endl;
        }

        void HandleLocationBaseFile(std::ifstream &ifstream)
        {
            std::cout << "loc base file!" << std::endl;

            {
                std::cout << "reading null head" << std::endl;
                uint32_t t, s;
                EAGLEye::readGeneric(ifstream, t);
                assert(t == 0x00034112);
                EAGLEye::readGeneric(ifstream, s);
                assert(s == 0x00000000);
            }
        }

        void HandleLocationFullFile(std::ifstream &ifstream)
        {
            std::cout << "loc full file!" << std::endl;
        }

        void HandleGlobalFile(std::ifstream &ifstream)
        {
            std::cout << "global file!" << std::endl;
        }

        void HandleGenericChunkedFile(std::ifstream &ifstream)
        {
            std::cout << "generic file!" << std::endl;
        }
    }
}
