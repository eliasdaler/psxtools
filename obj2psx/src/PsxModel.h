#pragma once

#include "Math.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>

struct PsxVert {
    Vec3<std::int16_t> pos;
    Vec2<std::uint8_t> uv;
};

struct PsxSubmesh {
    std::vector<std::array<PsxVert, 3>> triFaces;
    std::vector<std::array<PsxVert, 4>> quadFaces;
};

struct PsxModel {
    std::vector<PsxSubmesh> submeshes;
};

void writePsxModel(const PsxModel& model, const std::filesystem::path& path);
