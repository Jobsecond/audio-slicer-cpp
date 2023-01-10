cmake_minimum_required(VERSION 3.24)
project(audio_slicer)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(AUDIO_SLICER_CLI TRUE)
set(AUDIO_SLICER_GUI TRUE)

if(NOT(AUDIO_SLICER_CLI OR AUDIO_SLICER_GUI))
    MESSAGE(FATAL_ERROR "Must build one of CLI or GUI version, or both.")
    RETURN()
endif()

MESSAGE("The following targets will be built:")
if(AUDIO_SLICER_CLI)
    MESSAGE("- CLI")
endif()
if(AUDIO_SLICER_GUI)
    MESSAGE("- GUI")
endif()
MESSAGE("CMAKE_BUILD_TYPE is set to " ${CMAKE_BUILD_TYPE})

if(AUDIO_SLICER_CLI)
add_executable(audio_slicer_cli
        slicer.cpp main.cpp slicer.h)
endif()

if(AUDIO_SLICER_GUI)
add_executable(audio_slicer_gui WIN32
        slicer.cpp slicer.h main_gui.cpp gui/mainwindow.cpp gui/mainwindow.h gui/mainwindow.cpp gui/mainwindow.h gui/mainwindow.ui gui/workthread.cpp gui/workthread.h)
endif()


find_package(SndFile CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)

if(AUDIO_SLICER_GUI)
find_package(Qt5 REQUIRED Core Gui Widgets)
set(LIBS_GUI Qt5::Core Qt5::Widgets Qt5::Gui)
endif()

set(LIBS SndFile::sndfile argparse::argparse)

if(AUDIO_SLICER_CLI)
target_link_libraries(audio_slicer_cli PRIVATE ${LIBS})
endif()

if(AUDIO_SLICER_GUI)
target_link_libraries(audio_slicer_gui PRIVATE ${LIBS} ${LIBS_GUI})
set_target_properties(audio_slicer_gui PROPERTIES AUTOMOC TRUE)
endif()