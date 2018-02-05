#include <iostream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "eagldata.h"
#include "eaglenums.h"
#include "eaglutils.h"

#include "containers/carbon/CarbonChunkyContainer.h"
#include "containers/mw/MWChunkyContainer.h"

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
                    if (fileType == EAGLEye::FileType::NORMAL)
                    {
                        auto *container = new EAGLEye::Containers::MWChunkyContainer(stream);

                        container->ReadData();
                    }
                    break;
                }
                case 0x5:
                {
                    if (fileType == EAGLEye::FileType::NORMAL)
                    {
                        auto *container = new EAGLEye::Containers::CarbonChunkyContainer(stream);

                        container->ReadData();
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