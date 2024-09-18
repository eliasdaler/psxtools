#include "Json2PsxConverter.h"

#include "ModelJsonFile.h"
#include "PsxModel.h"

#include <format>
#include <iostream>

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

PsxModel jsonToPsxModel(const ModelJson& modelJson, const ConversionParams& params)
{
    PsxModel psxModel;
    for (const auto& object : modelJson.objects) {
        PsxSubmesh psxMesh;
        const auto& mesh = modelJson.meshes[object.mesh];
        const auto tm = object.transform.asMatrix();

        // TODO: support multiple materials?
        const auto& material = modelJson.materials[mesh.materials[0]];

        auto texWidth = material.imageData.width;
        auto texHeight = material.imageData.height;
        // std::cout << "tex:" << material.texture << " " << texWidth << " " << texHeight <<
        // std::endl;
        if (texWidth == 0 && texHeight == 0) {
            // temp hack
            texWidth = 128;
            texHeight = 128;
        }

        static const auto zeroUV = glm::vec2{};
        for (const auto& face : mesh.faces) {
            std::array<PsxVert, 4> psxFace;
            assert(face.vertices.size() <= 4);
            for (std::size_t i = 0; i < face.vertices.size(); ++i) {
                const auto& v = mesh.vertices[face.vertices[i]];
                // calculate world pos
                const auto pos = glm::vec3{tm * glm::vec4{v.position, 1.f}};

                psxFace[i].pos = {
                    .x = floatToInt16(pos.x, params.scale),
                    .y = floatToInt16(-pos.z, params.scale), // FLIP Y!
                    .z = floatToInt16(pos.y, params.scale), // FLIP Z!
                };

                float offset = 0;
                // float offset = 0;

                const auto& uv = face.uvs.empty() ? zeroUV : face.uvs[i];
                psxFace[i].uv = {
                    .x = (std::uint8_t)std::clamp(uv.x * (texWidth - offset), 0.f, 255.f),
                    // Y coord is flipped in UV
                    .y = (std::uint8_t)std::clamp((1 - uv.y) * (texHeight - offset), 0.f, 255.f),
                };

                psxFace[i].color =
                    {(std::uint8_t)(v.color.x / 2.f),
                     (std::uint8_t)(v.color.y / 2.f),
                     (std::uint8_t)(v.color.z / 2.f)};
            }

            if (face.vertices.size() == 3) {
                psxMesh.triFaces.push_back({psxFace[0], psxFace[2], psxFace[1]});
            } else {
                assert(face.vertices.size() == 4);
                // note the order - that's how PS1 quads work
                // psxMesh.quadFaces.push_back({psxFace[2], psxFace[1], psxFace[3], psxFace[0]});
                psxMesh.quadFaces.push_back({psxFace[0], psxFace[3], psxFace[1], psxFace[2]});
            }

            /* for (int i = 0; i < face.vertices.size(); ++i) {
                auto& v = face.vertices.size() == 3 ? psxMesh.triFaces.back()[i] :
                                                      psxMesh.quadFaces.back()[i];
                std::cout << v.pos.x << " " << v.pos.y << " " << v.pos.z << " | ";
                std::cout << (int)v.uv.x << " " << (int)v.uv.y << " | ";
                std::cout << (int)v.color.x << " " << (int)v.color.y << " " << (int)v.color.z
                          << std::endl;
            }
            std::cout << "____\n"; */
        }
        psxModel.submeshes.push_back(std::move(psxMesh));
    }
    return psxModel;
}
