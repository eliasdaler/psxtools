#include <cassert>

#include "TimCreateConfig.h"
#include "TimFile.h"
#include "TimFileCreator.h"
#include "TimFileWrite.h"
// #include "TimFileRead.h"

#include <fstream>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: timtool TIMS_JSON" << std::endl;
        return 1;
    }

    nlohmann::json root;
    std::ifstream file(argv[1]);
    assert(file.good());
    file >> root;

    bool hadErrors = false;
    const auto rootDir = std::filesystem::path(argv[1]).parent_path();
    for (const auto& cObj : root) {
        try {
            const auto config = readConfig(rootDir, cObj);
            const auto tim = createTimFile(config);
            std::cout << config.inputImage << " -> " << config.outputFile << std::endl;
            writeTimFile(tim, config.outputFile);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    if (hadErrors) {
        return 1;
    }
}
