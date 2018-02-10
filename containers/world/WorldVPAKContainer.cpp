#include "WorldVPAKContainer.h"

namespace EAGLEye
{
    namespace Containers
    {
        std::shared_ptr<void> WorldVPAKContainer::Get()
        {
            long fileLength = getFileLength(m_stream);

            MainHeader mainHeader{};
            readGeneric(m_stream, mainHeader);

            printf("VPAK: %d file(s) - name offset = 0x%08x\n", mainHeader.fileCount, mainHeader.fileTableLocation);

            FileHeader files[mainHeader.fileCount];

            for (int i = 0; i < mainHeader.fileCount; i++)
            {
                readGeneric(m_stream, files[i]);

                printf("File #%d - bin: 0x%08x | vlt: 0x%08x\n", files[i].fileNumber + 1, files[i].binLocation,
                       files[i].vltLocation);
            }

            m_stream.seekg(mainHeader.fileTableLocation);

            std::vector<BYTE> fileTable;
            fileTable.resize(static_cast<unsigned long>(mainHeader.fileTableLength));

            m_stream.read((char *) &fileTable[0], mainHeader.fileTableLength);

            std::map<int, std::string> nameTable;
            std::string name;
            int num = 0;

            for (unsigned char i : fileTable)
            {
                if (i == 0)
                {
                    printf("File #%d - %s\n", num + 1, name.c_str());
                    nameTable.insert({num, name});
                    name = "";
                    num++;
                } else
                {
                    name += (char) i;
                }
            }

            for (int i = 0; i < mainHeader.fileCount; i++)
            {
                m_stream.seekg(files[i].binLocation + 4);

                unsigned int numItems;
                readGeneric(m_stream, numItems);

                printf("%d\n", numItems);

                for (int j = 0; j < numItems; j++)
                {

                }
            }

            return nullptr;
        }

        size_t WorldVPAKContainer::ReadChunks(uint32_t totalSize)
        {
            return 0;
        }
    }
}