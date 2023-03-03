project(AudioSlicerCli VERSION ${BUILD_VERSION} LANGUAGES CXX)

set(TARGET_NAME ${PROJECT_NAME})
add_executable(${TARGET_NAME} main_cli.cpp)

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(${TARGET_NAME} PUBLIC "${SLICER_PATH}")
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_BINARY_DIR}/generated/)

target_link_libraries(${TARGET_NAME} PUBLIC AudioSlicerLib)
target_link_libraries(${TARGET_NAME} PRIVATE SndFile::sndfile argparse::argparse)

find_package(SndFile CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "AudioSlicerCLI")

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
  target_link_libraries(${TARGET_NAME} "stdc++fs")
endif()
