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

    void dumpBytesToFile(std::ifstream &stream, std::ofstream &output, size_t bytes)
    {
        auto curPos = stream.tellg();

        unsigned char buf[bytes];

        stream.read((char *) &buf[0], sizeof(buf));
        hexdump(output, buf, sizeof(buf));
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

    long AlignFS(std::ifstream &ifstream, int bytes)
    {
        long b2skip = PaddingAlign(ifstream.tellg(), bytes);
        ifstream.ignore(b2skip);
        return b2skip;
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

    uint32_t CRC32NFS::crcTable[256] = {};

    CRC32NFS::CRC32NFS()
    {
        uint32_t remainder;

        for (uint32_t dividend = 0; dividend < 256; ++dividend)
        {
            remainder = dividend << 24;

            for (uint32_t bit = 0; bit < 8; ++bit)
            {
                if ((remainder & 0x80000000u) > 0)
                {
                    remainder = (remainder << 1) ^ 0x04C11DB7u;
                } else
                {
                    remainder = (remainder << 1);
                }
            }

            crcTable[dividend] = remainder;
        }
    }

    uint32_t CRC32NFS::getHash(std::vector<BYTE> input)
    {
        uint32_t crc32 = 0x00000000u;

        if (input.size() >= 4)
        {
            int index = 0;
            crc32 = ~(uint32_t) ((input[index + 3]) | (input[index + 2] << 8) | (input[index + 1] << 16) |
                                 (input[index] << 24));
            index += 4;

            while (index < input.size())
            {
                crc32 = crcTable[crc32 >> 24] ^ ((crc32 << 8) | (input[index++]));
            }

            crc32 = ~crc32;
        }

        return crc32;
    }

    uint32_t JenkinsHash::getHash32(std::vector<BYTE> input, uint32_t magic)
    {
        int num = 0;
        size_t i = input.size();
        uint32_t a = 0x9E3779B9u, b = 0x9E3779B9u, c = magic;

        while (i >= 12)
        {
            a += BitConverter::ToUInt32(input.data(), num);
            b += BitConverter::ToUInt32(input.data(), num + 4);
            c += BitConverter::ToUInt32(input.data(), num + 8);
            mix32(a, b, c);
            num += 12;
            i -= 12;
        }

        c += (uint32_t) input.size();

        switch (i)
        {
            case 11:
                c += (uint) input[10 + num] << 24;
            case 10:
                c += (uint) input[9 + num] << 16;
            case 9:
                c += (uint) input[8 + num] << 8;
            case 8:
                b += (uint) input[7 + num] << 24;
            case 7:
                b += (uint) input[6 + num] << 16;
            case 6:
                b += (uint) input[5 + num] << 8;
            case 5:
                b += input[4 + num];
            case 4:
                a += (uint) input[3 + num] << 24;
            case 3:
                a += (uint) input[2 + num] << 16;
            case 2:
                a += (uint) input[1 + num] << 8;
            case 1:
                a += input[num];
                break;
            default:
                break;
        }

        return mix32_final(a, b, c);
    }

    unsigned long JenkinsHash::getHash64(std::vector<BYTE> input, unsigned long magic)
    {
        int num = 0;
        size_t i = input.size();
        unsigned long a = 0x9E3779B97F4A7C13uL, b = 0x9E3779B97F4A7C13uL, c = magic;

        while (i >= 24)
        {
            a += BitConverter::ToUInt64(input.data(), num);
            b += BitConverter::ToUInt64(input.data(), num + 8);
            c += BitConverter::ToUInt64(input.data(), num + 16);
            mix64(a, b, c);
            num += 24;
            i -= 24;
        }

        c += input.size();

        switch (i)
        {
            // NOTE: C# doesn't allow fallthroughs, so as a workaround we use goto's instead.
            case 23:
                c += (unsigned long) input[22] << 56;
            case 22:
                c += (unsigned long) input[21] << 48;
            case 21:
                c += (unsigned long) input[20] << 40;
            case 20:
                c += (unsigned long) input[19] << 32;
            case 19:
                c += (unsigned long) input[18] << 24;
            case 18:
                c += (unsigned long) input[17] << 16;
            case 17:
                c += (unsigned long) input[16] << 8;
            case 16:
                b += (unsigned long) input[15] << 56;
            case 15:
                b += (unsigned long) input[14] << 48;
            case 14:
                b += (unsigned long) input[13] << 40;
            case 13:
                b += (unsigned long) input[12] << 32;
            case 12:
                b += (unsigned long) input[11] << 24;
            case 11:
                b += (unsigned long) input[10] << 16;
            case 10:
                b += (unsigned long) input[9] << 8;
            case 9:
                b += input[8];
            case 8:
                a += (unsigned long) input[7] << 56;
            case 7:
                a += (unsigned long) input[6] << 48;
            case 6:
                a += (unsigned long) input[5] << 40;
            case 5:
                a += (unsigned long) input[4] << 32;
            case 4:
                a += (unsigned long) input[3] << 24;
            case 3:
                a += (unsigned long) input[2] << 16;
            case 2:
                a += (unsigned long) input[1] << 8;
            case 1:
                a += input[0];
                break;
            default:
                break;
        }

        // micro-optimization: avoids ref overhead and skips 1 assignment by having a separate function for this.
        return mix64_final(a, b, c);
    }

    uint32_t JenkinsHash::getHash32(const char *data, uint32_t magic)
    {
        return JenkinsHash::getHash32(std::vector<BYTE>(data, data + sizeof(data)), magic);
    }

    unsigned long JenkinsHash::getHash64(const char *data, unsigned long magic)
    {
        return JenkinsHash::getHash64(std::vector<BYTE>(data, data + sizeof(data)), magic);
    }

    void JenkinsHash::mix32(uint32_t &a, uint32_t &b, uint32_t &c)
    {
        a = c >> 13 ^ ( a - b - c );
        b = a <<  8 ^ ( b - c - a );
        c = b >> 13 ^ ( c - a - b );
        a = c >> 12 ^ ( a - b - c );
        b = a << 16 ^ ( b - c - a );
        c = b >>  5 ^ ( c - a - b );
        a = c >>  3 ^ ( a - b - c );
        b = a << 10 ^ ( b - c - a );
        c = b >> 15 ^ ( c - a - b );
    }

    void JenkinsHash::mix64(unsigned long &a, unsigned long &b, unsigned long &c)
    {
        a = c >> 43 ^ ( a - b - c );
        b = a <<  9 ^ ( b - c - a );
        c = b >>  8 ^ ( c - a - b );
        a = c >> 38 ^ ( a - b - c );
        b = a << 23 ^ ( b - c - a );
        c = b >>  5 ^ ( c - a - b );
        a = c >> 35 ^ ( a - b - c );
        b = a << 49 ^ ( b - c - a );
        c = b >> 11 ^ ( c - a - b );
        a = c >> 12 ^ ( a - b - c );
        b = a << 18 ^ ( b - c - a );
        c = b >> 22 ^ ( c - a - b );
    }

    uint32_t JenkinsHash::mix32_final(uint32_t &a, uint32_t &b, uint32_t &c)
    {
        a = c >> 13 ^ ( a - b - c );
        b = a <<  8 ^ ( b - c - a );
        c = b >> 13 ^ ( c - a - b );
        a = c >> 12 ^ ( a - b - c );
        b = a << 16 ^ ( b - c - a );
        c = b >>  5 ^ ( c - a - b );
        a = c >>  3 ^ ( a - b - c );
        b = a << 10 ^ ( b - c - a );
        return b >> 15 ^ ( c - a - b );
    }

    unsigned long JenkinsHash::mix64_final(unsigned long &a, unsigned long &b, unsigned long &c)
    {
        a = c >> 43 ^ ( a - b - c );
        b = a <<  9 ^ ( b - c - a );
        c = b >>  8 ^ ( c - a - b );
        a = c >> 38 ^ ( a - b - c );
        b = a << 23 ^ ( b - c - a );
        c = b >>  5 ^ ( c - a - b );
        a = c >> 35 ^ ( a - b - c );
        b = a << 49 ^ ( b - c - a );
        c = b >> 11 ^ ( c - a - b );
        a = c >> 12 ^ ( a - b - c );
        b = a << 18 ^ ( b - c - a );
        return b >> 22 ^ ( c - a - b );
    }

    int BinHash::Hash(std::vector<BYTE> bytes)
    {
        int i = -1;

        for (unsigned char byte : bytes)
        {
            i = byte + 33 * i;
        }

        return i;
    }

    int BinHash::Hash(const char *string)
    {
        return Hash(std::vector<BYTE>(string, string + sizeof(string)));
    }
}