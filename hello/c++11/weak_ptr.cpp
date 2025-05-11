#include <iostream>

#include "spdlog/spdlog.h"

int main(int, char **)
{
    std::string name("hello", 30);
    int a = 1024;
    std::weak_ptr<int> wp;

    SPDLOG_INFO("start: {} {} {}", a, "hello", name);

    return 0;
}
