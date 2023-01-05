cmake_minimum_required(VERSION 3.24)
project(audio_slicer)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(audio_slicer
        slicer.cpp main.cpp slicer.h)
find_package(SndFile CONFIG REQUIRED)
target_link_libraries(audio_slicer PRIVATE SndFile::sndfile)

find_package(xtensor CONFIG REQUIRED)
target_link_libraries(audio_slicer PRIVATE xtensor)

find_package(argparse CONFIG REQUIRED)
target_link_libraries(audio_slicer PRIVATE argparse::argparse)
