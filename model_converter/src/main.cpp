#include <cassert>
#include <charconv>
#include <filesystem>
#include <iostream>

#include "Json2PsxConverter.h"
#include "ModelJsonFile.h"
#include "Obj2PsxConverter.h"
#include "ObjFile.h"
#include "PsxModel.h"

namespace
{}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cerr << "Usage: obj2psx OBJFILE OUTFILE ASSETDIR [SCALE]\n";
        return 1;
    }

    // parse obj
    const auto inputFilePath = std::filesystem::path{argv[1]};
    const auto ouputFilePath = std::filesystem::path{argv[2]};
    const auto assetDirPath = std::filesystem::path{argv[3]};

    std::cout << "in: " << inputFilePath << std::endl;
    std::cout << "out: " << ouputFilePath << std::endl;
    std::cout << "asset dir: " << assetDirPath << std::endl;

    float scale = 1.f;
    if (argc == 5) {
        std::string_view scaleArg{argv[4]};
        std::from_chars(scaleArg.data(), scaleArg.data() + scaleArg.size(), scale);
        std::cout << "scale: " << scale << std::endl;
    }

    // convert
    ConversionParams conversionParams{
        .scale = scale,
    };

    if (inputFilePath.extension() == ".obj") {
        const auto objModel = parseObjFile(inputFilePath);
        const auto psxModel = objToPsxModel(objModel, conversionParams);
        writePsxModel(psxModel, ouputFilePath);
    } else if (inputFilePath.extension() == ".json") {
        const auto modelJson = parseJsonFile(inputFilePath, assetDirPath);
        const auto psxModel = jsonToPsxModel(modelJson, conversionParams);
        writePsxModel(psxModel, ouputFilePath);
    }

    std::cout << "Done!" << std::endl;
}
