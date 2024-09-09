#include "TimFileWrite.h"

#include "TimFile.h"
#include <fstream>

namespace
{
std::uint32_t toXY32(std::uint16_t x, std::uint16_t y)
{
    return (y << 16) | x;
}
}

template<typename T>
void binaryWrite(std::ostream& os, const T& val)
{
    os.write(reinterpret_cast<const char*>(&val), sizeof(T));
}

void writeTimFile(const TimFile& timFile, const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);

    // magic
    binaryWrite(file, TimFile::MAGIC);

    // flag
    auto flag = static_cast<std::uint32_t>(timFile.pmode);
    if (timFile.hasClut) {
        flag |= 0x8; // 3rd bit
    }
    binaryWrite(file, flag);

    // clut
    assert(timFile.hasClut && "TODO: handle files without CLUT");
    assert(timFile.pmode == TimFile::PMode::Clut4Bit);

    if (timFile.hasClut) {
        std::uint32_t bnum = 12u; // bnum + DXDY + WH
        if (timFile.pmode == TimFile::PMode::Clut4Bit) {
            bnum += timFile.cluts16.size() * 16 * 2;
        } else if (timFile.pmode == TimFile::PMode::Clut8Bit) {
            bnum += timFile.cluts16.size() * 256 * 2;
        }

        binaryWrite(file, bnum);
        binaryWrite(file, toXY32(timFile.clutDX, timFile.clutDY));
        binaryWrite(file, toXY32(timFile.clutW, timFile.clutH));

        if (timFile.pmode == TimFile::PMode::Clut4Bit) {
            for (const auto& clut : timFile.cluts16) {
                for (const auto& c : clut.colors) {
                    binaryWrite(file, to16BitColor(c));
                }
            }
        } else if (timFile.pmode == TimFile::PMode::Clut8Bit) {
            // TOOD
        }
    }

    { // pixel data
        std::uint32_t bnum = 12u; // bnum + DXDY + WH + W*H
        assert(timFile.pmode == TimFile::PMode::Clut4Bit);
        if (timFile.pmode == TimFile::PMode::Clut4Bit) {
            bnum += (timFile.pixW * timFile.pixH) * 2;
        }

        binaryWrite(file, bnum);
        binaryWrite(file, toXY32(timFile.pixDX, timFile.pixDY));
        binaryWrite(file, toXY32(timFile.pixW, timFile.pixH));

        if (timFile.pmode == TimFile::PMode::Clut4Bit) {
            for (std::size_t i = 0; i < timFile.pixW * timFile.pixH * 4; i += 4) {
                std::uint16_t pd =
                    (timFile.pixelsIdx[i + 3] << 12) | (timFile.pixelsIdx[i + 2] << 8) |
                    (timFile.pixelsIdx[i + 1] << 4) | (timFile.pixelsIdx[i + 0] << 0);
                binaryWrite(file, pd);
            }
        }
    }
}
