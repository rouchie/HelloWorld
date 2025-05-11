cmake_minimum_required(VERSION 3.29)
project(hello)

set(CMAKE_CXX_STANDARD 11)

find_package(unofficial-concurrentqueue CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(LIBS
    unofficial::concurrentqueue::concurrentqueue
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# 获取当前目录下所有 .cpp 文件（替换为你的目录路径）
file(GLOB SOURCES "c++11/*.cpp" "other/*.cpp")

# 遍历每个文件并生成可执行文件
foreach(source_file ${SOURCES})
    # 提取文件名（不带路径和扩展名）
    get_filename_component(executable_name ${source_file} NAME_WE)
    # 生成可执行文件
    add_executable(${executable_name} ${source_file})
    target_link_libraries(${executable_name} PRIVATE ${LIBS})
endforeach()
