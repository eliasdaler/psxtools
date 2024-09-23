#include <cassert>

#include "TimCreateConfig.h"
#include "TimFile.h"
#include "TimFileCreator.h"
#include "TimFileWrite.h"
// #include "TimFileRead.h"

#include <CLI/CLI.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[])
{
    CLI::App cliApp{};

    std::filesystem::path timsJsonPath;
    cliApp.add_option("TIMSJSON", timsJsonPath, "TIM description JSON file")
        ->required()
        ->check(CLI::ExistingFile);

    try {
        cliApp.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        std::exit(cliApp.exit(e));
    }

    nlohmann::json root;
    std::ifstream file(timsJsonPath);
    assert(file.good());
    file >> root;

    bool hadErrors = false;
    const auto rootDir = std::filesystem::path(timsJsonPath).parent_path();
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
