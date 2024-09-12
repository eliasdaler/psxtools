#include "PsxModel.h"

#include <fstream>

namespace
{

template<typename T>
void binaryWrite(std::ostream& os, const T& val)
{
    os.write(reinterpret_cast<const char*>(&val), sizeof(T));
}
}

void writePsxModel(const PsxModel& model, const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);
    binaryWrite(file, static_cast<std::uint16_t>(model.submeshes.size()));
    for (const auto& submesh : model.submeshes) {
        // write triangles
        binaryWrite(file, static_cast<std::uint16_t>(submesh.triFaces.size()));
        for (const auto& face : submesh.triFaces) {
            for (int i = 0; i < 3; ++i) {
                binaryWrite(file, face[i].pos.x);
                binaryWrite(file, face[i].pos.y);
                binaryWrite(file, face[i].pos.z);
                binaryWrite(file, face[i].uv.x);
                binaryWrite(file, face[i].uv.y);
            }
        };
        // write quads
        binaryWrite(file, static_cast<std::uint16_t>(submesh.quadFaces.size()));
        for (const auto& face : submesh.quadFaces) {
            for (int i = 0; i < 4; ++i) {
                binaryWrite(file, face[i].pos.x);
                binaryWrite(file, face[i].pos.y);
                binaryWrite(file, face[i].pos.z);
                binaryWrite(file, face[i].uv.x);
                binaryWrite(file, face[i].uv.y);
            }
        };
    }
}
