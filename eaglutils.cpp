#include "eaglutils.h"

namespace EAGLEye
{
    BYTE readByte(std::ifstream &stream)
    {
        return (BYTE) stream.get();
    }

    void dumpBytes(std::ifstream &stream, size_t bytes)
    {
        auto curPos = stream.tellg();

        unsigned char buf[bytes];

        stream.read((char *) &buf[0], sizeof(buf));
        hexdump(stdout, buf, sizeof(buf));
        stream.seekg(curPos);
    }

    short BitConverter::ToInt16(const BYTE *bytes, int offset)
    {
        auto result = (int16_t) ((int) bytes[offset] & 0xff);
        result |= ((int) bytes[offset + 1] & 0xff) << 8;
        return (int16_t) (result & 0xffff);
    }

    uint16_t BitConverter::ToUInt16(const BYTE *bytes, int offset)
    {
        auto result = (int) bytes[offset + 1] & 0xff;
        result |= ((int) bytes[offset] & 0xff) << 8;
        return (uint16_t) (result & 0xffff);
    }

    int32_t BitConverter::ToInt32(const BYTE *bytes, int offset)
    {
        auto result = (int) bytes[offset] & 0xff;
        result |= ((int) bytes[offset + 1] & 0xff) << 8;
        result |= ((int) bytes[offset + 2] & 0xff) << 16;
        result |= ((int) bytes[offset + 3] & 0xff) << 24;
        return result;
    }

    uint32_t BitConverter::ToUInt32(const BYTE *bytes, int offset)
    {
        auto result = static_cast<uint32_t>((int) bytes[offset] & 0xff);
        result |= ((int) bytes[offset + 1] & 0xff) << 8;
        result |= ((int) bytes[offset + 2] & 0xff) << 16;
        result |= ((int) bytes[offset + 3] & 0xff) << 24;
        return result & 0xFFFFFFFF;
    }

    uint64_t BitConverter::ToUInt64(const BYTE *bytes, int offset)
    {
        uint64_t result = 0;

        for (int i = 0; i <= 56; i += 8)
        {
            result |= ((int) bytes[offset++] & 0xff) << i;
        }

        return result;
    }

    std::vector<BYTE> BitConverter::GetBytes(int value)
    {
        std::vector<BYTE> bytes;
        bytes.resize(4);
        bytes[0] = (BYTE) (value >> 24);
        bytes[1] = (BYTE) (value >> 16);
        bytes[2] = (BYTE) (value >> 8);
        bytes[3] = (BYTE) (value);
        return bytes;
    }

    std::vector<BYTE> BitConverter::GetBytes(long value)
    {
        std::vector<BYTE> bytes;
        bytes.resize(4);
        bytes[0] = (BYTE) (value >> 24);
        bytes[1] = (BYTE) (value >> 16);
        bytes[2] = (BYTE) (value >> 8);
        bytes[3] = (BYTE) (value);
        return bytes;
    }

    /**
 * Ported to C++ from https://github.com/MWisBest/OpenNFSTools/blob/master/LibNFS/Compression/JDLZ.cs
 * Credit to MWIsBest for the C# code
 *
 * @param input
 * @return
 */
    std::vector<BYTE> JDLZ::decompress(std::vector<BYTE> input)
    {
        if (input.size() < HEADER_SIZE
            || input[0] != 'J'
            || input[1] != 'D'
            || input[2] != 'L'
            || input[3] != 'Z'
            || input[4] != 0x02)
        {
            throw std::runtime_error("Invalid header");
        }

        int flags1 = 1, flags2 = 1;
        int t, length;
        int inPos = HEADER_SIZE, outPos = 0;

        // TODO: Can we always trust the header's stated length?

        std::vector<BYTE> output;
        output.resize(static_cast<unsigned long>(BitConverter::ToInt32(input.data(), 8)));

        while ((inPos < input.size()) && (outPos < output.size()))
        {
            if (flags1 == 1)
            {
                flags1 = input[inPos++] | 0x100;
            }
            if (flags2 == 1)
            {
                flags2 = input[inPos++] | 0x100;
            }

            if ((flags1 & 1) == 1)
            {
                if ((flags2 & 1) == 1) // 3 to 4098(?) iterations, backtracks 1 to 16(?) bytes
                {
                    // length max is 4098(?) (0x1002), assuming input[inPos] and input[inPos + 1] are both 0xFF
                    length = (input[inPos + 1] | ((input[inPos] & 0xF0) << 4)) + 3;
                    // t max is 16(?) (0x10), assuming input[inPos] is 0xFF
                    t = (input[inPos] & 0x0F) + 1;
                } else // 3(?) to 34(?) iterations, backtracks 17(?) to 2064(?) bytes
                {
                    // t max is 2064(?) (0x810), assuming input[inPos] and input[inPos + 1] are both 0xFF
                    t = (input[inPos + 1] | ((input[inPos] & 0xE0) << 3)) + 17;
                    // length max is 34(?) (0x22), assuming input[inPos] is 0xFF
                    length = (input[inPos] & 0x1F) + 3;
                }

                inPos += 2;

                for (int i = 0; i < length; ++i)
                {
                    output[outPos + i] = output[outPos + i - t];
                }

                outPos += length;
                flags2 >>= 1;
            } else
            {
                if (outPos < output.size())
                {
                    output[outPos++] = input[inPos++];
                }
            }
            flags1 >>= 1;
        }
        return output;
    }
}