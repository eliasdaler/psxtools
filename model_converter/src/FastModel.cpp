#include "FastModel.h"

#include <cassert>
#include <fstream>

#include <FsUtil.h>

#include <cmath>

void writeFastModel(const FastModel& model, const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);
    assert(file.good());
    // write verts
    fsutil::binaryWrite(file, static_cast<std::uint16_t>(model.vertexPos.size()));
    file.write(
        reinterpret_cast<const char*>(model.vertexPos.data()),
        sizeof(Vec3<std::int16_t>) * model.vertexPos.size());
    // write prims
    fsutil::binaryWrite(file, model.triNum);
    fsutil::binaryWrite(file, model.quadNum);
    file.write(reinterpret_cast<const char*>(model.primData.data()), model.primData.size());
}
