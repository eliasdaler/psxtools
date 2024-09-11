#include <cassert>

#include "ImageLoader.h"
#include "TimFile.h"
#include "TimFileRead.h"
#include "TimFileWrite.h"

#include <unordered_set>

struct ColorHash {
    size_t operator()(const Color32& color) const { return (size_t)toU32Color(color); }
};

void fillClut(const ImageData& imageData, std::array<Color32, 16>& clut)
{
    std::unordered_set<Color32, ColorHash> colors;
    for (const auto& p : imageData.pixels) {
        if (!colors.contains(p)) {
            colors.insert(p);
        }
    }
    assert(colors.size() == 16);

    int colorNum = 0;
    for (const auto& c : colors) {
        clut[colorNum++] = c;
        printColor(c);
        printColorQuant(c);
    }
}

int main(int argc, char* argv[])
{
    /* const auto data = util::loadImage(argv[1]);
    std::cout << data.width << " " << data.height << std::endl;

    std::array<Color32, 16> clut;
    fillClut(data, clut); */

    const auto timFile = readTimFile(argv[1]);
    writeTimFile(timFile, argv[2]);

#if 0
    auto timFile2 = readTIM(argv[2]);
    assert(timFile.pmode == timFile2.pmode);
    assert(timFile.hasCLUT == timFile.hasCLUT);
#endif
}
