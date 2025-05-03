#ifndef __APCPP_SOLO_GEN_H__
#define __APCPP_SOLO_GEN_H__

#include <string>
#include <string_view>
#include <filesystem>

namespace sologen {
    constexpr std::u8string_view yaml_folder = u8"solo_yaml"; 
    constexpr std::u8string_view yaml_filename = u8"solo.yaml"; 
    bool generate(const std::filesystem::path& yaml_dir, const std::filesystem::path& output_dir);
}

#endif
