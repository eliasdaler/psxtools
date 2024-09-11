#include "TimFileCreator.h"

#include "ImageLoader.h"
#include "TimFile.h"

#include <unordered_map>
#include <unordered_set>

namespace
{

void fillClut(const ImageData& imageData, TimFile::Clut& clut)
{
    std::unordered_set<ColorR5G5B5STP> colors;
    for (const auto& p : imageData.pixels) {
        auto quant = to16BitColor(p);
        if (!colors.contains(quant)) {
            colors.insert(quant);
        }
    }

    assert(colors.size() == 16 || colors.size() <= 256);
    const auto clutSize = colors.size() == 16 ? 16 : 256;
    clut.colors.resize(clutSize);
    std::size_t colorNum = 0;
    for (const auto& c : colors) {
        clut.colors[colorNum] = c;
        ++colorNum;
    }
}

} // end of anonymous namespace

TimFile createTimFile(const TimConfig& config)
{
    if (!std::filesystem::exists(config.inputImage)) {
        throw std::runtime_error("File doesn't exist " + config.inputImage.string());
    }

    const auto data = util::loadImage(config.inputImage);
    if (data.pixelsRaw == nullptr) {
        throw std::runtime_error("Failed to open image " + config.inputImage.string());
    }

    TimFile tim;

    // fill CLUT
    // TODO: quantization
    // TODO: Direct15bit
    tim.cluts.resize(1);
    fillClut(data, tim.cluts[0]);
    if (tim.cluts[0].colors.size() == 16) {
        tim.pmode = TimFile::PMode::Clut4Bit;
        tim.hasClut = true;
    } else if (tim.cluts[0].colors.size() == 256) {
        tim.pmode = TimFile::PMode::Clut8Bit;
        tim.hasClut = true;
    }

    if (tim.hasClut) {
        const auto clutNumColors = tim.clutW = TimFile::getNumColorsInClut(tim.pmode);
        tim.clutDX = config.clutDX;
        tim.clutDY = config.clutDY;

        tim.clutW = clutNumColors;
        tim.clutH = 1;
    }

    auto wScale = 1;
    if (tim.pmode == TimFile::PMode::Clut8Bit) {
        wScale = 2;
    } else if (tim.pmode == TimFile::PMode::Clut4Bit) {
        wScale = 4;
    }

    tim.pixDX = config.pixDX;
    tim.pixDY = config.pixDY;

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
