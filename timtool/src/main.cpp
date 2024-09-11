#include <cassert>

#include "ImageLoader.h"
#include "TimFile.h"
#include "TimFileRead.h"
#include "TimFileWrite.h"

#include <filesystem>
#include <unordered_map>
#include <unordered_set>

struct ColorHash {
    size_t operator()(const Color32& color) const { return (size_t)toU32Color(color); }
};

void fillClut(const ImageData& imageData, TimFile::Clut& clut)
{
    std::unordered_set<ColorR5G5B5STP> colors;
    for (const auto& p : imageData.pixels) {
        auto quant = to16BitColor(p);
        if (!colors.contains(quant)) {
            colors.insert(quant);
        }
    }
    assert(colors.size() == clut.colors.size());

    int colorNum = 0;
    for (const auto& c : colors) {
        clut.colors[colorNum++] = c;
    }
}

TimFile makeTimFile(const std::filesystem::path& path)
{
    const auto data = util::loadImage(path);
    std::cout << data.width << " " << data.height << std::endl;

    TimFile tim;
    tim.pmode = TimFile::PMode::Clut4Bit;

    tim.hasClut = (tim.pmode == TimFile::PMode::Clut4Bit || tim.pmode == TimFile::PMode::Clut8Bit);
    if (tim.hasClut) {
        const auto clutNumColors = tim.clutW = TimFile::getNumColorsInClut(tim.pmode);
        tim.clutDX = 0;
        tim.clutDY = 483;

        tim.clutW = clutNumColors;
        tim.clutH = 1;

        tim.cluts.resize(1);
        tim.cluts[0].colors.resize(clutNumColors);
        fillClut(data, tim.cluts[0]);
    }

    auto wScale = 1;
    if (tim.pmode == TimFile::PMode::Clut8Bit) {
        wScale = 2;
    } else if (tim.pmode == TimFile::PMode::Clut4Bit) {
        wScale = 4;
    }

    tim.pixDX = 512;
    tim.pixDY = 0;

    tim.pixW = data.width / wScale;
    tim.pixH = data.height;

    tim.pixelsIdx.reserve(data.width * data.height);

    std::unordered_map<ColorR5G5B5STP, std::uint8_t> clutRL;
    int colorIndex{0};
    for (const auto& c : tim.cluts[0].colors) {
        clutRL.emplace(c, colorIndex);
        ++colorIndex;
    }

    for (const auto& p : data.pixels) {
        tim.pixelsIdx.push_back(clutRL.at(to16BitColor(p)));
    }

    return tim;
}

int main(int argc, char* argv[])
{
    const auto tim = readTimFile(argv[1]);
    // const auto tim = makeTimFile(argv[1]);
    writeTimFile(tim, argv[2]);

#if 0
    auto timFile2 = readTIM(argv[2]);
    assert(timFile.pmode == timFile2.pmode);
    assert(timFile.hasCLUT == timFile.hasCLUT);
#endif
}
