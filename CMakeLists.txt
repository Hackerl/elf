cmake_minimum_required(VERSION 3.17)
project(elf)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(
        elf
        src/reader.cpp
        src/header.cpp
        src/segment.cpp
        src/section.cpp
        src/symbol.cpp
)

target_include_directories(elf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)