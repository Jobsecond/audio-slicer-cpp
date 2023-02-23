cmake_minimum_required(VERSION 3.24)
project(audio_slicer)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(AUDIO_SLICER_CLI "Build CLI version" ON)
option(AUDIO_SLICER_GUI "Build GUI version" ON)
option(BUILD_MACOSX_BUNDLE "Build macOS app bundle" ON)

if(WIN32)
    set(GUI_TYPE WIN32)
elseif(APPLE AND BUILD_MACOSX_BUNDLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif()

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
    add_executable(audio_slicer_gui ${GUI_TYPE}
            slicer.cpp slicer.h main_gui.cpp gui/mainwindow.cpp gui/mainwindow.h gui/mainwindow.cpp gui/mainwindow.h gui/mainwindow.ui gui/workthread.cpp gui/workthread.h)
endif()


find_package(SndFile CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)

if(AUDIO_SLICER_GUI)
    find_package(Qt5 REQUIRED Core Gui Widgets)
    set(LIBS_GUI Qt5::Core Qt5::Widgets Qt5::Gui)
endif()

set(LIBS SndFile::sndfile argparse::argparse)

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    set(LIBS "stdc++fs" ${LIBS})
endif()

if(AUDIO_SLICER_CLI)
    target_link_libraries(audio_slicer_cli PRIVATE ${LIBS})
endif()

if(AUDIO_SLICER_GUI)
    target_link_libraries(audio_slicer_gui PRIVATE ${LIBS} ${LIBS_GUI})
    set_target_properties(audio_slicer_gui PROPERTIES AUTOMOC TRUE)
endif()

if(APPLE AND AUDIO_SLICER_GUI AND BUILD_MACOSX_BUNDLE)
    set_target_properties(audio_slicer_gui PROPERTIES
    BUILD_RPATH "@executable_path/../Frameworks"
    INSTALL_RPATH "@executable_path/../Frameworks"    
    )
endif()
