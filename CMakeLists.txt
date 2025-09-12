cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/build/conan_toolchain.cmake")
project(RigTransport
    VERSION 0.0.0
    DESCRIPTION "Rig Transport Library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CTest)
include(Catch)
include(GenerateExportHeader)
find_package(Catch2 REQUIRED)
enable_testing()

add_subdirectory(Common)
add_subdirectory(Cpp)
