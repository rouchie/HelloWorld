#include <iostream>

#include "spdlog/spdlog.h"

void hello() {
    SPDLOG_INFO("hello");
}

int main(int, char **)
{
    SPDLOG_INFO("start");
    return 0;
}

