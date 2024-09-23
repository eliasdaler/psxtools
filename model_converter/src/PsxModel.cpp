#include "PsxModel.h"

#include <fstream>
#include <iostream>

#include <FsUtil.h>

void writePsxModel(const PsxModel& model, const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);
    fsutil::binaryWrite(file, static_cast<std::uint16_t>(model.submeshes.size()));

    for (const auto& submesh : model.submeshes) {
        // std::cout << "triFaces = " << submesh.triFaces.size() << " "
        //          << "quadFaces = " << submesh.quadFaces.size() << std::endl;

        // write num of tris and quads
        fsutil::binaryWrite(file, static_cast<std::uint16_t>(submesh.triFaces.size()));
        fsutil::binaryWrite(file, static_cast<std::uint16_t>(submesh.quadFaces.size()));

        // write tris
        for (const auto& face : submesh.triFaces) {
            for (int i = 0; i < 3; ++i) {
                fsutil::binaryWrite(file, face[i].pos.x);
                fsutil::binaryWrite(file, face[i].pos.y);
                fsutil::binaryWrite(file, face[i].pos.z);
                fsutil::binaryWrite(file, face[i].uv.x);
                fsutil::binaryWrite(file, face[i].uv.y);
                fsutil::binaryWrite(file, face[i].color.x);
                fsutil::binaryWrite(file, face[i].color.y);
                fsutil::binaryWrite(file, face[i].color.z);
                fsutil::binaryWrite(file, face[i].color.z); // padding
            }
        };
        // write quads
        for (const auto& face : submesh.quadFaces) {
            for (int i = 0; i < 4; ++i) {
                fsutil::binaryWrite(file, face[i].pos.x);
                fsutil::binaryWrite(file, face[i].pos.y);
                fsutil::binaryWrite(file, face[i].pos.z);
                fsutil::binaryWrite(file, face[i].uv.x);
                fsutil::binaryWrite(file, face[i].uv.y);
                fsutil::binaryWrite(file, face[i].color.x);
                fsutil::binaryWrite(file, face[i].color.y);
                fsutil::binaryWrite(file, face[i].color.z);
                fsutil::binaryWrite(file, face[i].color.z); // padding
            }
        };
    }
}
