#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"

#define THROW(ABC, ...) {                                                                \
    std::string e = fmt::format("throw [{}:{}] " ABC, __FILE__, __LINE__, ##__VA_ARGS__);\
    SPDLOG_ERROR("{}", e);                                                               \
    throw std::runtime_error(e);                                                         \
}

// 16 进制打印
// spdlog::to_hex
// {:X}	以大写字母打印十六进制数，默认是小写显示 0000: 09 0A 0B 0C FF FF
// {:s}	不带间隔符显示 0000: 090a0b0cffff
// {:n}	不要将输出分割成行 09 0a 0b 41 0d 4b ff ff
// {:a}	如果未设置:n，则显示 ASCII 字符（对于可打印的 ASCII 字符）	0000 : 09 0a 0b 41 0d 4b ff ff  ...A.K..
// {:p}	不要在每行开始时打印位置	09 0a 0b 41 0d 4b ff ff