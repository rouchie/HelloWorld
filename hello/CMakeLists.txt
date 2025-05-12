cmake_minimum_required(VERSION 3.29)
project(hello)

set(CMAKE_CXX_STANDARD 11)

include_directories(
    ${PROJECT_SOURCE_DIR}
)

find_package(unofficial-concurrentqueue CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(workflow CONFIG REQUIRED)
find_package(Libevent CONFIG REQUIRED)

set(LIBS
    unofficial::concurrentqueue::concurrentqueue
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    libevent::core libevent::extra
)

#################################################################################
file(GLOB BASE_SOURCE "leopard/*.cpp" "http/*.cpp")
add_library(leopard STATIC ${BASE_SOURCE})
target_link_libraries(leopard PRIVATE ${LIBS} workflow)

#################################################################################
file(GLOB DEMO_SOURCE "demo/*.cpp")
add_executable(demo ${DEMO_SOURCE})
target_link_libraries(demo PRIVATE leopard ${LIBS} OpenSSL::SSL OpenSSL::Crypto workflow)

#################################################################################
# 获取当前目录下所有 .cpp 文件（替换为你的目录路径）
file(GLOB EXES_SOURCES "c++11/*.cpp" "other/*.cpp")
# 遍历每个文件并生成可执行文件
foreach(source_file ${EXES_SOURCES})
    # 提取文件名（不带路径和扩展名）
    get_filename_component(executable_name ${source_file} NAME_WE)
    # 生成可执行文件
    add_executable(${executable_name} ${source_file})
    target_link_libraries(${executable_name} PRIVATE ${LIBS})
endforeach()
