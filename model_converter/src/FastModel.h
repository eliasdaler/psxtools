#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

#include "Math.h"

struct FastModel {
    std::vector<Vec3<std::int16_t>> vertexPos;
    std::vector<Vec3<std::uint8_t>> vertexColors;
    std::uint16_t triNum;
    std::uint16_t quadNum;
    std::vector<std::uint8_t> primData;
};

void writeFastModel(const FastModel& model, const std::filesystem::path& path);
