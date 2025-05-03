#include <memory>
#include <filesystem>
#include <string>
#include <format>
#include <fstream>
#include <iostream>

#include "apcpp-glue.h"
#include "apcpp-solo-gen.h"

std::string yaml_text{};

namespace fs = std::filesystem;
RECOMP_DLL_FUNC(rando_yaml_init) {
    yaml_text.clear();
}

template <int arg_index>
std::string _arg_string_length(uint8_t* rdram, recomp_context* ctx, u32 len) {
    PTR(char) str = _arg<arg_index, PTR(char)>(rdram, ctx);

    std::string ret{};
    ret.resize(len);

    for (size_t i = 0; i < len; i++) {
        ret[i] = (char)MEM_B(str, i);
    }

    return ret;
}

RECOMP_DLL_FUNC(rando_yaml_puts) {
    std::string to_append = _arg_string_length<0>(rdram, ctx, RECOMP_ARG(u32, 1));
    yaml_text += to_append;
}

RECOMP_DLL_FUNC(rando_yaml_finalize) {
    std::u8string savepath_str = RECOMP_ARG_U8STR(0);
    std::cout << yaml_text;

    fs::path savepath(savepath_str);
    fs::path yaml_dir = savepath.parent_path() / sologen::yaml_folder;

    std::filesystem::create_directories(yaml_dir);    
    fs::path yaml_path = yaml_dir / sologen::yaml_filename;

    std::ofstream out(yaml_path);
    try {
        out << yaml_text;
    } catch (std::exception e){
        std::cout << e.what();
    }
}
