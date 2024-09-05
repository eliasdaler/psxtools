#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
constexpr int POS_ATTR = 0;
constexpr int UV_ATTR = 1;
constexpr int NORMAL_ATTR = 2;

template<typename T>
struct Vec2 {
    T x, y;
};

template<typename T>
struct Vec3 {
    T x, y, z;
};

struct Face {
    using VertexIndices = std::array<int, 4>;
    std::array<VertexIndices, 4> indices;
    int numVertices;
    int numAttrs;
};

struct Mesh {
    std::string name;
    std::vector<Face> faces;
};

struct ObjModel {
    std::vector<Vec3<float>> vertices;
    std::vector<Vec3<float>> normals;
    std::vector<Vec2<float>> uvs;
    std::vector<Mesh> meshes;
};

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

bool isBlankLine(const std::string& str)
{
    for (const auto& c : str) {
        if (!std::isspace(c)) {
            return false;
        }
    }
    return true;
}

float stringToFloat(const std::string& str)
{
    float res;
    std::from_chars(str.data(), str.data() + str.size(), res);
    return res;
}

std::vector<std::string> splitString(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ss(s);
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void rtrim(std::string& s)
{
    s.erase(
        std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); })
            .base(),
        s.end());
}

Face parseFaceString(const std::string& line, int linenum)
{
    const auto ts = splitString(line, ' '); // line = f attr1/attr2/attr3 ...

    Face face{};
    face.numVertices = static_cast<int>(ts.size() - 1);
    assert(face.numVertices == 3 || face.numVertices == 4);

    for (std::size_t i = 1; i < ts.size(); ++i) {
        const auto ts2 = splitString(ts[i], '/'); // ts[i] = attr1/attr2/attr3
        if (face.numAttrs == 0) {
            face.numAttrs = ts2.size();
        } else if (ts2.size() != face.numAttrs) {
            std::cout << "Line: " << linenum
                      << ", skipping face: inconsistent number of attributes. First seen "
                      << face.numAttrs << ", but now got " << ts.size() << std::endl;
            return {};
        }
        assert(face.numAttrs != 0);
        for (int attr = 0; attr < face.numAttrs; ++attr) {
            face.indices[i - 1][attr] = std::stoi(ts2[attr]);
            assert(face.indices[i - 1][attr] >= 0);
        }
    }

    return face;
}

ObjModel parseObjFile(const std::filesystem::path& path)
{
    std::ifstream objfile(path);
    if (!objfile.good()) {
        std::cerr << "Failed to open obj file " << path << std::endl;
        std::exit(1);
    }

    std::string line;

    bool firstSubmesh = true;

    ObjModel model;
    Mesh mesh;

    int linenum = 0;
    while (std::getline(objfile, line)) {
        ++linenum;
        if (line.starts_with("#")) {
            continue;
        }
        if (line.empty() || isBlankLine(line)) {
            continue;
        }
        if (line.starts_with("mtllib")) {
            continue; // TODO: mtl
        }

        if (line.starts_with("o ")) {
            const auto ts = splitString(line, ' ');
            if (!firstSubmesh) {
                model.meshes.push_back(std::move(mesh));
            } else {
                firstSubmesh = false;
            }
            mesh.name = ts[1];
            rtrim(mesh.name);
        } else if (line.starts_with("v ")) {
            const auto ts = splitString(line, ' ');
            model.vertices.push_back(Vec3<float>{
                .x = stringToFloat(ts[1]),
                .y = stringToFloat(ts[2]),
                .z = stringToFloat(ts[3]),
            });
        } else if (line.starts_with("vt ")) {
            const auto ts = splitString(line, ' ');
            model.uvs.push_back(Vec2<float>{
                .x = stringToFloat(ts[1]),
                .y = stringToFloat(ts[2]),
            });
        } else if (line.starts_with("vn ")) {
            const auto ts = splitString(line, ' ');
            model.normals.push_back(Vec3<float>{
                .x = stringToFloat(ts[1]),
                .y = stringToFloat(ts[2]),
                .z = stringToFloat(ts[3]),
            });
        } else if (line.starts_with("f ")) {
            Face face = parseFaceString(line, linenum);
            if (face.numVertices != 0) {
                mesh.faces.push_back(std::move(face));
            }
        }
    }
    if (!mesh.name.empty()) {
        model.meshes.push_back(std::move(mesh));
    }

    for (const auto& mesh : model.meshes) {
        std::cout << "Submesh: " << mesh.name << ", num faces: " << mesh.faces.size() << std::endl;
    }

    return model;
}

static constexpr float SCALE_FACTOR = 1.5f;
std::int16_t floatToInt16(float v)
{
    return (std::int16_t)std::clamp(v * SCALE_FACTOR, (float)INT16_MIN, (float)INT16_MAX);
}

std::uint8_t uvToInt8(float v)
{
    return (std::int8_t)std::clamp(v * 255.f, (float)0, (float)255);
}

PsxModel objToPsxModel(const ObjModel& objModel)
{
    PsxModel psxModel;
    for (const auto& mesh : objModel.meshes) {
        PsxSubmesh psxMesh;
        for (const auto& face : mesh.faces) {
            std::array<PsxVert, 4> psxFace;
            for (std::size_t i = 0; i < face.numVertices; ++i) {
                const auto& pos = objModel.vertices.at(face.indices[i][POS_ATTR] - 1);
                psxFace[i].pos = {
                    .x = floatToInt16(pos.x),
                    .y = floatToInt16(-pos.y), // FLIP Y!
                    .z = floatToInt16(pos.z),
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

template<typename T>
void binaryWrite(std::ostream& os, const T& val)
{
    os.write(reinterpret_cast<const char*>(&val), sizeof(T));
}

void writePsxModel(const PsxModel& model, const std::filesystem::path& path)
{
    std::ofstream file(path, std::ios::binary);
    binaryWrite(file, static_cast<std::uint16_t>(model.submeshes.size()));
    for (const auto& submesh : model.submeshes) {
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
    }
}

}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: obj2psx OBJFILE OUTFILE\n";
        return 1;
    }

    std::cout << "in: " << argv[1] << std::endl;
    std::cout << "out: " << argv[2] << std::endl;

    const auto objModel = parseObjFile(argv[1]);
    const auto psxModel = objToPsxModel(objModel);
    writePsxModel(psxModel, argv[2]);
    std::cout << "Done!" << std::endl;
}
