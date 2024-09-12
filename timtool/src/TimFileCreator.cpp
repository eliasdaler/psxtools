#include "TimFileCreator.h"

#include "ImageLoader.h"
#include "TimCreateConfig.h"
#include "TimFile.h"

#include <unordered_map>
#include <unordered_set>

#include <ImageMagick-6/Magick++.h>

namespace
{

Color16 to16BitColor(const Color32& c, const TimCreateConfig& config)
{
    if (!config.useAlphaChannel && c == config.transparencyColor) {
        // Image has no alpha channel, use config.transparencyColor for transparency
        return 0;
    }

    if (config.useAlphaChannel && c.a <= config.alphaThreshold) {
        return 0;
    }

    bool setSTP = false;
    if (config.useAlphaChannel && c.a < config.stpThreshold) {
        setSTP = true;
    }

    static const auto black = Color32{0, 0, 0, 255};
    if (c == black) {
        if (config.nonTransparentBlack) {
            return (config.setSTPOnBlack ? (1 << 15) : (0 << 15)) | 1;
        }
        if (config.setSTPOnBlack) {
            setSTP = true;
        }
    } else if (config.setSTPOnNonBlack) {
        setSTP = true;
    }

    return (setSTP ? (1 << 15) : (0 << 15)) | (from8BitTo5Bit(c.b) << 10) |
           (from8BitTo5Bit(c.g) << 5) | (from8BitTo5Bit(c.r));
}

std::unordered_set<Color16> findUniqueColors(
    const ImageData& imageData,
    const TimCreateConfig& config)
{
    std::unordered_set<Color16> colors;
    for (const auto& p : imageData.pixels) {
        auto quant = to16BitColor(p, config);
        if (!colors.contains(quant)) {
            colors.insert(quant);
        }
    }
    return colors;
}

// FIXME: ImageMagick is kinda heavy, but it'll do for now...
void quantizeImage(ImageData& data)
{
    // copy to Magick::Image
    Magick::Image image(Magick::Geometry(data.width, data.height), "transparent");
    Magick::Pixels view(image);
    auto* pixels = view.get(0, 0, data.width, data.height);
    for (ssize_t row = 0; row < data.height; ++row) {
        for (ssize_t column = 0; column < data.width; ++column) {
            const auto& p = data.pixels[row * data.width + column];
            pixels->red = p.r * 255;
            pixels->green = p.g * 255;
            pixels->blue = p.b * 255;
            pixels->opacity = p.a * 255;
            ++pixels;
        }
    }

    // set quantization params (TODO: allow to change dither method?
    image.quantizeColors(256);
    image.quantizeDither(true);
    image.quantizeDitherMethod(Magick::DitherMethod::FloydSteinbergDitherMethod);
    image.quantize();

    // copy back to ImageData
    const auto* packet = image.getConstPixels(0, 0, data.width, data.height);
    for (std::size_t i = 0; i < data.width * data.height; ++i) {
        data.pixels[i] = Color32{
            .r = (std::uint8_t)(packet->red / 255),
            .g = (std::uint8_t)(packet->green / 255),
            .b = (std::uint8_t)(packet->blue / 255),
            .a = (std::uint8_t)(packet->opacity / 255),
        };
        ++packet;
    }
}

} // end of anonymous namespace

TimFile createTimFile(const TimCreateConfig& config)
{
    if (!std::filesystem::exists(config.inputImage)) {
        throw std::runtime_error("File doesn't exist " + config.inputImage.string());
    }

    auto data = util::loadImage(config.inputImage);
    if (data.pixelsRaw == nullptr) {
        throw std::runtime_error("Failed to open image " + config.inputImage.string());
    }

    if (data.width > 256 || data.height > 256) {
        throw std::runtime_error(std::format(
            "Image {} dimensions bigger than 256x256 ({}x{})",
            config.inputImage.string(),
            data.width,
            data.height));
    }

    TimFile tim;

    if (!config.direct15Bit) {
        tim.cluts.resize(1);

        auto colors = findUniqueColors(data, config);

        if (colors.size() > 256) {
            quantizeImage(data);
            colors = findUniqueColors(data, config);
        }

        // TODO: multiple cluts?
        // fill clut
        auto& clut = tim.cluts[0];
        assert(colors.size() == 16 || colors.size() <= 256);
        const auto clutSize = colors.size() <= 16 ? 16 : 256;
        clut.colors.resize(clutSize);

        std::size_t colorNum = 0;
        for (const auto& c : colors) {
            clut.colors[colorNum] = c;
            ++colorNum;
        }

        if (tim.cluts[0].colors.size() == 16) {
            tim.pmode = TimFile::PMode::Clut4Bit;
            tim.hasClut = true;
        } else if (tim.cluts[0].colors.size() == 256) {
            tim.pmode = TimFile::PMode::Clut8Bit;
            tim.hasClut = true;
        }
    } else {
        tim.pmode = TimFile::PMode::Direct15Bit;
    }

    std::cout << config.inputImage << " " << tim.cluts[0].colors.size() << std::endl;

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

    if (tim.hasClut) {
        tim.pixelsIdx.reserve(data.width * data.height);

        std::unordered_map<Color16, std::uint8_t> clutRL;
        int colorIndex{0};
        for (const auto& c : tim.cluts[0].colors) {
            clutRL.emplace(c, colorIndex);
            ++colorIndex;
        }

        for (const auto& p : data.pixels) {
            tim.pixelsIdx.push_back(clutRL.at(to16BitColor(p, config)));
        }
    } else {
        tim.pixels.reserve(data.pixels.size());
        for (const auto& p : data.pixels) {
            tim.pixels.push_back(to16BitColor(p, config));
        }
    }

    return tim;
}
