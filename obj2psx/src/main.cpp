#include <cassert>
#include <charconv>
#include <filesystem>
#include <iostream>

#include "Obj2PsxConverter.h"
#include "ObjFile.h"
#include "PsxModel.h"

namespace
{}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: obj2psx OBJFILE OUTFILE [SCALE]\n";
        return 1;
    }

    std::cout << "in: " << argv[1] << std::endl;
    std::cout << "out: " << argv[2] << std::endl;

    // parse obj
    const auto objModel = parseObjFile(argv[1]);

    float scale = 1.f;
    if (argc == 4) {
        std::string_view scaleArg{argv[3]};
        std::from_chars(scaleArg.data(), scaleArg.data() + scaleArg.size(), scale);
        std::cout << "scale: " << scale << std::endl;
    }

    // convert
    ConversionParams conversionParams{
        .scale = scale,
    };
    const auto psxModel = objToPsxModel(objModel, conversionParams);

    // write model
    writePsxModel(psxModel, argv[2]);

    std::cout << "Done!" << std::endl;
}
