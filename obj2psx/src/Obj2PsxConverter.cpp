#include "Obj2PsxConverter.h"

#include "ObjFile.h"
#include "PsxModel.h"

#include <algorithm>
#include <cassert>

namespace
{
std::int16_t floatToInt16(float v, float scaleFactor)
{
    return (std::int16_t)std::clamp(v * scaleFactor, (float)INT16_MIN, (float)INT16_MAX);
}

std::uint8_t uvToInt8(float v)
{
    return (std::int8_t)std::clamp(v * 256.f, 0.f, 255.f);
}
}

PsxModel objToPsxModel(const ObjModel& objModel, const ConversionParams& params)
{
    PsxModel psxModel;
    for (const auto& mesh : objModel.meshes) {
        PsxSubmesh psxMesh;
        for (const auto& face : mesh.faces) {
            std::array<PsxVert, 4> psxFace;
            for (std::size_t i = 0; i < face.numVertices; ++i) {
                const auto& pos = objModel.vertices.at(face.indices[i][POS_ATTR] - 1);
                psxFace[i].pos = {
                    .x = floatToInt16(pos.x, params.scale),
                    .y = floatToInt16(-pos.y, params.scale), // FLIP Y!
                    .z = floatToInt16(pos.z, params.scale),
                };

                const auto& uv = objModel.uvs.at(face.indices[i][UV_ATTR] - 1);
                psxFace[i].uv = {
                    .x = uvToInt8(uv.x),
                    .y = uvToInt8(1.f - uv.y), // Y coord is flipped in UV
                };
            }
            if (face.numVertices == 3) {
                psxMesh.triFaces.push_back({psxFace[0], psxFace[1], psxFace[2]});
            } else {
                assert(face.numVertices == 4);
                psxMesh.quadFaces.push_back({psxFace[0], psxFace[1], psxFace[2], psxFace[3]});
            }
        }
        psxModel.submeshes.push_back(std::move(psxMesh));
    }
    return psxModel;
}
