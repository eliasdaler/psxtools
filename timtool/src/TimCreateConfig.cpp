#include "TimCreateConfig.h"

#include <nlohmann/json.hpp>

namespace
{
bool getBoolOrElse(const nlohmann::json& j, const std::string_view keyName, bool defaultValue)
{
    if (auto it = j.find(keyName); it != j.end()) {
        assert(it.value().is_boolean());
        return it.value().get<bool>();
    }
    return defaultValue;
}

std::uint8_t getUInt8OrElse(
    const nlohmann::json& j,
    const std::string_view keyName,
    std::uint8_t defaultValue)
{
    if (auto it = j.find(keyName); it != j.end()) {
        assert(
            it.value().is_number_integer() && it.value().get<std::int64_t>() >= 0 &&
            it.value().get<std::int64_t>() <= 255);
        return it.value().get<std::uint8_t>();
    }
    return defaultValue;
}
}

TimCreateConfig readConfig(const std::filesystem::path& rootDir, const nlohmann::json& j)
{
    TimCreateConfig config{};
    config.inputImage = rootDir / j.at("input").get<std::filesystem::path>();
    config.outputFile = rootDir / j.at("output").get<std::filesystem::path>();

    // read clut DX and DY
    if (const auto it = j.find("clut"); it != j.end()) {
        const auto& clutObj = it.value();
        assert(clutObj.is_array());
        assert(clutObj.size() == 2);
        config.clutDX = clutObj[0];
        config.clutDY = clutObj[1];
    }

    // read pix DX and DY
    if (const auto it = j.find("pix"); it != j.end()) {
        const auto& pixObj = it.value();
        assert(pixObj.is_array());
        assert(pixObj.size() == 2);
        config.pixDX = pixObj[0];
        config.pixDY = pixObj[1];
    }

    // read transparenct color
    if (auto it = j.find("transparency_color"); it != j.end()) {
        const auto& pixObj = it.value();
        assert(pixObj.is_array());
        assert(pixObj.size() == 3);
        config.transparencyColor.r = pixObj[0];
        config.transparencyColor.g = pixObj[1];
        config.transparencyColor.b = pixObj[2];
    }

    config.direct15Bit = getBoolOrElse(j, "direct", false);

    // read alpha stuff
    config.setSTPOnNonBlack = getBoolOrElse(j, "set_stp_on_non_black", false);
    config.setSTPOnBlack = getBoolOrElse(j, "set_stp_on_black", false);
    config.nonTransparentBlack = getBoolOrElse(j, "non_transparent_black", true);

    config.useAlphaChannel = getBoolOrElse(j, "use_alpha", true);
    config.alphaThreshold = getUInt8OrElse(j, "alpha_threshold", 0);
    config.stpThreshold = getUInt8OrElse(j, "stp_threshold", 255);

    // edge case - alpha channel is not used and we want
    // transparency as black - default value for non_transparent_black
    // should be "false", otherwise we'll replace it with (1, 0, 0, STP)!
    if (!config.useAlphaChannel) {
        if (config.transparencyColor == Color32{0, 0, 0, 255} &&
            !j.contains("non_transparent_black")) {
            config.nonTransparentBlack = false;
        }
    }

    return config;
}