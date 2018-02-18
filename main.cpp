#include <iostream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "eagldata.h"
#include "eaglenums.h"
#include "eaglutils.h"

#include "chunk/ChunkIO.h"
#include "containers/mw/MWChunkyContainer.h"
#include "containers/mw/MWLocationContainer.h"

#include "containers/carbon/CarbonChunkyContainer.h"

#include "containers/world/WorldChunkyContainer.h"
#include "containers/world/WorldCompressedContainer.h"
#include "containers/world/WorldVPAKContainer.h"

#include "containers/uc/UndercoverChunkyContainer.h"
#include "containers/uc/UndercoverLocationContainer.h"
#include "containers/ug2/UG2ChunkyContainer.h"
#include "containers/prostreet/PSChunkyContainer.h"
#include "containers/prostreet/PSLocationContainer.h"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

/**
 * hacky way of determining the "type" of a file
 * (generally as a way to have custom behavior for different files)
 * @param ifstream
 * @return
 */
EAGLEye::FileType determineFileType(std::ifstream &ifstream, fs::path &path)
{
    auto curPos = (long) ifstream.tellg();

    BYTE buf[4];
    EAGLEye::readGeneric(ifstream, buf);

    ifstream.seekg(curPos);

    if ((buf[0] == 'J' && buf[1] == 'D' && buf[2] == 'L' && buf[3] == 'Z')
        || (buf[0] == 0x88 && buf[1] == 0x33 && buf[2] == 0x11 && buf[3] == 0x66))
    {
        return EAGLEye::FileType::COMPRESSED;
    }

    if (buf[0] == 'V' && buf[1] == 'P' && buf[2] == 'A' && buf[3] == 'K')
    {
        return EAGLEye::FileType::VPAK;
    }

    return EAGLEye::FileType::NORMAL;
}

int main(int argc, char **argv)
{
    try
    {
        po::options_description desc("Allowed options");
        std::string inputFilename;
        std::string gameName;
        std::string action;
        std::string inputFileType;
        desc.add_options()
                ("help,h", "Show description")
                ("file,f", po::value<std::string>(&inputFilename)->required(), "Input file")
                ("action,a", po::value<std::string>(&action), "Action")
                ("type,t", po::value<std::string>(&inputFileType), "File Type")
                ("game,g", po::value<std::string>(&gameName)->required(), "Game ID");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, const_cast<const char *const *>(argv), desc), vm);
        po::notify(vm);

        std::map<std::string, int>::const_iterator foundGame_;

        std::string gameId_ = boost::algorithm::to_upper_copy(vm["game"].as<std::string>());

        if ((foundGame_ = EAGLEye::gameMap.find(gameId_)) == EAGLEye::gameMap.end())
        {
            std::cerr << "Unknown game! Possible values: "
                      << boost::algorithm::join(EAGLEye::extract_keys<std::string, int>(EAGLEye::gameMap), ", ")
                      << std::endl;
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

            EAGLEye::FileType fileType = determineFileType(stream, filePath);

            switch (foundGame_->second)
            {
                case 0x2:
                {
                    if (action.empty())
                    {
                        if (fileType == EAGLEye::FileType::NORMAL)
                        {
                            auto *container = new EAGLEye::Containers::MWChunkyContainer(stream);

                            container->Get();
                        }
                    } else if (action == "read-locbundle")
                    {
                        auto *container = new EAGLEye::Containers::MWLocationContainer(stream, filePath);

                        container->Get();
                    }
                    break;
                }
                case 0x5:
                {
                    if (action.empty())
                    {
                        if (fileType == EAGLEye::FileType::NORMAL)
                        {
                            auto *container = new EAGLEye::Containers::CarbonChunkyContainer(stream, filePath);

                            container->Get();
                        }
                    } else if (action == "extract")
                    {
                        EAGLEye::Chunks::ExtractChunksToFolder(filePath, boost::filesystem::absolute("chunk-dump"));
                    } else if (action == "extract-stream")
                    {
                        boost::filesystem::path dumpPath = boost::filesystem::absolute("chunk-dump-5a");
                        EAGLEye::Chunks::ExtractStreamChunks(filePath, dumpPath);
                    } else if (action == "combine-stream")
                    {
                        boost::filesystem::path dumpPath = boost::filesystem::absolute("chunk-dump-5a");
                        boost::filesystem::path outPath = boost::filesystem::path(filePath.parent_path());
                        EAGLEye::Chunks::CombineStreamChunks(filePath, dumpPath, outPath);
                    } else if (action == "pack")
                    {
                        boost::filesystem::path repackPath = boost::filesystem::path(filePath.parent_path());
                        repackPath.append(filePath.filename().stem().string());
                        repackPath += "-repack.BUN";

                        EAGLEye::Chunks::CombineFromFiles(boost::filesystem::absolute("chunk-dump"),
                                                          filePath.filename(), repackPath);
                    }

                    break;
                }
                case 0x5a:
                {
                    if (action.empty())
                    {
                        if (fileType == EAGLEye::FileType::NORMAL)
                        {
                            auto *container = new EAGLEye::Containers::WorldChunkyContainer(stream);

                            container->Get();
                        } else if (fileType == EAGLEye::FileType::COMPRESSED)
                        {
                            auto *container = new EAGLEye::Containers::WorldCompressedContainer(stream, filePath);

                            container->Get();
                        } else if (fileType == EAGLEye::FileType::VPAK)
                        {
                            auto *container = new EAGLEye::Containers::WorldVPAKContainer(stream);

                            container->Get();
                        }
                    } else if (action == "extract")
                    {
                        EAGLEye::Chunks::ExtractChunksToFolder(filePath, boost::filesystem::absolute("chunk-dump"));
                    } else if (action == "pack")
                    {
                        EAGLEye::Chunks::CombineFromFiles(boost::filesystem::absolute("chunk-dump"),
                                                          filePath.filename(),
                                                          boost::filesystem::path(filePath.stem().string() +
                                                                                  std::string("-repack.BUN")));
                    }
                    break;
                }
                case 0x4: // UG2
                {
                    if (action.empty())
                    {
                        if (fileType == EAGLEye::FileType::NORMAL)
                        {
                            auto *container = new EAGLEye::Containers::UG2ChunkyContainer(stream);

                            container->Get();
                        }
                    }
                    break;
                }
                case 0x6: // PS
                {
                    if (action.empty())
                    {
                        if (fileType == EAGLEye::FileType::NORMAL)
                        {
                            auto *container = new EAGLEye::Containers::PSChunkyContainer(stream);

                            container->Get();
                        }
                    } else if (action == "read-locbundle")
                    {
                        auto *container = new EAGLEye::Containers::PSLocationContainer(stream, filePath);

                        container->Get();
                    }
                    break;
                }
                case 0x8: // UC
                {
                    if (action.empty())
                    {
                        if (fileType == EAGLEye::FileType::NORMAL)
                        {
                            auto *container = new EAGLEye::Containers::UndercoverChunkyContainer(stream);

                            container->Get();
                        }
                    } else if (action == "read-locbundle")
                    {
                        auto *container = new EAGLEye::Containers::UndercoverLocationContainer(stream, filePath);

                        container->Get();
                    }
                    break;
                }
                default:
                    break;
            }
        }
    } catch (const po::error &ex)
    {
        std::cerr << ex.what() << '\n';
    }

    return 0;
}