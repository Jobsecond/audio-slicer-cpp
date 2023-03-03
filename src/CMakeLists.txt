project(AudioSlicer VERSION ${BUILD_VERSION} LANGUAGES CXX)

set(TARGET_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(AUDIO_SLICER_CLI "Build CLI version" ON)
option(AUDIO_SLICER_GUI "Build GUI version" ON)
option(BUILD_MACOSX_BUNDLE "Build macOS app bundle" ON)


add_subdirectory(app)
add_subdirectory(slicer)

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
    add_subdirectory(cli)
endif()

if(AUDIO_SLICER_GUI)
    add_subdirectory(gui)
endif()
