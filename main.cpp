#include <iostream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "eaglutils.h"
#include "chunkbase.h"
#include "GameSupport/MW.h"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

/**
 * hacky way of determining the "type" of a file
 * (generally as a way to have custom behavior for different files)
 * @param ifstream
 * @return
 */
EAGLEye::FileType determineFileType(std::ifstream& ifstream)
{
    auto curPos = (long) ifstream.tellg();

    uint32_t t;
    EAGLEye::readGeneric(ifstream, t);

    if (t == 0x00000000)
    {
        uint32_t s;
        EAGLEye::readGeneric(ifstream, s);
        ifstream.ignore(s);
        EAGLEye::readGeneric(ifstream, t);
    }

    ifstream.seekg(curPos);

    if (t == 0x00034112)
    {
        return EAGLEye::LOCATION_BASE;
    }

    if (t == 0x80134000 || t == 0xB3300000)
    {
        return EAGLEye::LOCATION_FULL;
    }

    if (t == 0x00039000)
    {
        return EAGLEye::LANGUAGE;
    }

    return EAGLEye::GENERIC_CHUNKED;
}

int main(int argc, char **argv)
{
    try
    {
        po::options_description desc("Allowed options");
        std::string inputFilename;
        std::string gameName;
        desc.add_options()
                ("help,h", "Show description")
                ("file,f", po::value<std::string>(&inputFilename), "Input file")
                ("game,g", po::value<std::string>(&gameName), "Game ID");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, const_cast<const char *const *>(argv), desc), vm);
        po::notify(vm);

        std::map<std::string, int>::const_iterator foundGame_;

        std::string gameId_ = boost::algorithm::to_upper_copy(vm["game"].as<std::string>());

        if ((foundGame_ = EAGLEye::gameMap.find(gameId_)) == EAGLEye::gameMap.end())
        {
            std::cerr << "Unknown game! Possible values: " << boost::algorithm::join(EAGLEye::extract_keys(EAGLEye::gameMap), ", ") << std::endl;
            return 1;
        }

        std::cout << "game: " << foundGame_->first << " (" << foundGame_->second << ")" << std::endl;


        fs::path filePath = fs::absolute(inputFilename);

        std::cout << filePath.string() << std::endl;

        if (!fs::exists(filePath))
        {
            std::cout << "Can't find file!" << std::endl;
            return 1;
        }

        if (!fs::is_regular_file(filePath))
        {
            std::cout << "Invalid file! Not a regular file..." << std::endl;
            return 1;
        }

        {
            std::ifstream stream(filePath.string(), std::ios::in | std::ios::binary);

            EAGLEye::FileType fileType = determineFileType(stream);

            switch (foundGame_->second)
            {
                case 0x2:
                    EAGLEye::MW::HandleFile(filePath, stream, fileType);
                    break;
                default:break;
            }
        }
    } catch (const po::error &ex)
    {
        std::cerr << ex.what() << '\n';
    }


//    std::pair<std::string, std::pair<int, std::string>> matchedGame;
//    bool didMatch = false;
//
//    for (auto &gamePair : gameMap)
//    {
//        if (baseBunFile.find(gamePair.first) != std::string::npos)
//        {
//            matchedGame = gamePair;
//            didMatch = true;
//            break;
//        }
//    }
//
//    if (!didMatch)
//    {
//        std::cout << "Couldn't determine game type" << std::endl;
//        return 1;
//    }
//
//    int gameId = matchedGame.second.first;
//
//    if (gameId != 0x2)
//    {
//        std::cout << "Unsupported game" << std::endl;
//        return 1;
//    }
//
//    std::cout << "Guessing game: " << matchedGame.second.second << std::endl;

    {
//        std::ifstream baseBunStream(baseBunPath.string(), std::ios::in | std::ios::binary);
//
//        {
//            std::cout << "reading null head" << std::endl;
//            uint32_t t, s;
//            EAGLEye::readGeneric(baseBunStream, t);
//            assert(t == 0x00034112);
//            EAGLEye::readGeneric(baseBunStream, s);
//            assert(s == 0x00000000);
//        }
//
//        uintmax_t fileSize = fs::file_size(baseBunPath);
//
//        for (int i = 1; i < 0xFFFF && baseBunStream.tellg() < fileSize; i++)
//        {
//            std::cout << std::endl;
//            uint32_t t, s;
//            EAGLEye::readGeneric(baseBunStream, t);
//            EAGLEye::readGeneric(baseBunStream, s);
//
//            printf("0x%08x\n", t);
//            EAGLEye::dumpBytes(baseBunStream, 256);
//
//            switch (t)
//            {
//                case 0x80034147: // BCHUNK_TRACKPATH
//                    switch (gameId)
//                    {
//                        case 0x2:
//                            EAGLEye::MW::ParseTrackPathChunk(baseBunStream, t, s);
//                        default:
//                            break;
//                    }
//
//                    break;
//                case 0x00034110: // BCHUNK_TRACKSTREAMER_0
//                    switch (gameId)
//                    {
//                        case 0x2:
//                        {
//                            auto sectionsChunk = EAGLEye::MW::ParseTrackStreamerSectionsChunk(baseBunStream, t, s);
//
//                            std::cout << "Sections (" << sectionsChunk->sections.size() << "):" << std::endl;
//                            for (auto &section : sectionsChunk->sections)
//                            {
//                                std::cout << section.id << std::endl;
//                            }
//
//                            break;
//                        }
//                        default:
//                            break;
//                    }
//                    break;
//                case 0x00e34010: // BCHUNK_EAGLANIMATIONS
//                    switch (gameId)
//                    {
//                        case 0x2:
//                            EAGLEye::MW::ParseAnimationsChunk(baseBunStream, t, s);
//                        default:
//                            break;
//                    }
//
//                    break;
//                case 0x00034158: // BCHUNK_VISIBLESECTION v1
//                    switch (gameId)
//                    {
//                        case 0x2:
//                            EAGLEye::MW::ParseVisibleSectionChunk(baseBunStream, t, s);
//                        default:
//                            break;
//                    }
//                    break;
//                default:
//                    baseBunStream.ignore(s);
//                    break;
//            }
//        }
    }

    return 0;
}