#pragma once

#include <cassert>
#include <filesystem>
#include <vector>

#include "Color.h"

struct TimFile {
    static constexpr std::uint32_t MAGIC = 0x00000010;

    enum class PMode {
        Clut4Bit,
        Clut8Bit,
        Direct15Bit,
        Direct24Bit,
        Mixed,
    };

    template<std::size_t N>
    struct Clut {
        std::array<Color32, N> colors;
    };

    PMode pmode;

    bool hasClut{false};
    std::uint16_t clutDX;
    std::uint16_t clutDY;
    std::uint16_t clutW;
    std::uint16_t clutH;
    std::vector<Clut<16>> cluts16;
    std::vector<Clut<256>> cluts256;

    std::uint16_t pixDX;
    std::uint16_t pixDY;
    std::uint16_t pixW;
    std::uint16_t pixH;
    std::vector<Color32> pixels;
    std::vector<std::uint8_t> pixelsIdx;
};

void writeTimFile(const TimFile& timFile, const std::filesystem::path& path);

template<std::size_t N>
std::uint8_t getClutIndex(const TimFile::Clut<N> clut, const Color32& c)
{
    for (std::uint8_t i = 0; i < N; ++i) {
        if (clut.colors[i].r == c.r && clut.colors[i].g == c.g && clut.colors[i].b == c.b &&
            clut.colors[i].a == c.a) {
            return i;
        }
    }
    assert("Color not found in CLUT!");
    return 0;
}
