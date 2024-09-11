#pragma once

#include <cstdint>
#include <filesystem>

struct TimFile;

struct TimConfig {
    std::filesystem::path inputImage;
    std::uint16_t clutDX;
    std::uint16_t clutDY;
    std::uint16_t pixDX;
    std::uint16_t pixDY;
};

TimFile createTimFile(const TimConfig& config);
