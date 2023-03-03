cmake_minimum_required(VERSION 3.20)

#set(VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
set(DEFAULT_VERSION "0.0.0")

#if(EXISTS "${VERSION_FILE}")
#    file(READ "${VERSION_FILE}" BUILD_VERSION)
#    string(STRIP "${BUILD_VERSION}" BUILD_VERSION)
#else()
#    message(WARNING "File ${VERSION_FILE} not found. Using the default version number: ${DEFAULT_VERSION}")
#    string(STRIP "${DEFAULT_VERSION}" BUILD_VERSION)
#endif()
include(Config.cmake)

project(AudioSlicer VERSION ${BUILD_VERSION} LANGUAGES CXX)

add_subdirectory(src)
