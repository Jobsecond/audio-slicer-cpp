cmake_minimum_required(VERSION 3.24)
project(audio_slicer)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#add_executable(audio_slicer
#        slicer.cpp main.cpp slicer.h main_gui.cpp gui/mainwindow.cpp gui/mainwindow.h gui/mainwindow.cpp gui/mainwindow.h gui/mainwindow.ui gui/workthread.cpp gui/workthread.h)
add_executable(audio_slicer
        slicer.cpp slicer.h main_gui.cpp gui/mainwindow.cpp gui/mainwindow.h gui/mainwindow.cpp gui/mainwindow.h gui/mainwindow.ui gui/workthread.cpp gui/workthread.h)
find_package(SndFile CONFIG REQUIRED)
target_link_libraries(audio_slicer PRIVATE SndFile::sndfile)

find_package(argparse CONFIG REQUIRED)
target_link_libraries(audio_slicer PRIVATE argparse::argparse)

find_package(Qt5 REQUIRED Core Gui Widgets)
target_link_libraries(audio_slicer PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui)

set_target_properties(audio_slicer PROPERTIES AUTOMOC TRUE)