#include <iostream>

#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"

int main(int, char **)
{
    std::string name = "我就是要写点中文进去";

    nlohmann::json j;
    j["name"] = name;
    j["age"] = 18;

    SPDLOG_INFO("{}", j.dump());

    return 0;
}
