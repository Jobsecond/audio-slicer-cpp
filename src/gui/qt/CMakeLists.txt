project(AudioSlicerGuiQt VERSION ${BUILD_VERSION} LANGUAGES CXX)

set(TARGET_NAME ${PROJECT_NAME})

if(WIN32)
    set(ICON_FILE_PATH "${APP_ICON_WINDOWS_RC_FILE}")
elseif(APPLE AND BUILD_MACOSX_BUNDLE)
    set(ICON_FILE_PATH "${APP_ICON_MACOS_ICNS_FILE}")
    cmake_path(GET ICON_FILE_PATH FILENAME ICON_FILE_NAME)
    set_source_files_properties(${ICON_FILE_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

add_executable(${TARGET_NAME} "${GUI_TYPE}"
    main_qt.cpp mainwindow.cpp mainwindow.h mainwindow.cpp workthread.cpp workthread.h
    ${ICON_FILE_PATH}
)

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(APPLE AND BUILD_MACOSX_BUNDLE)
    set_target_properties(
        ${TARGET_NAME} PROPERTIES
        BUILD_RPATH "@executable_path/../Frameworks"
        INSTALL_RPATH "@executable_path/../Frameworks"
        MACOSX_BUNDLE_ICON_FILE "${ICON_FILE_NAME}"
        MACOSX_BUNDLE_NAME "${MACOSX_BUNDLE_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${BUILD_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "${COPYRIGHT_TEXT}"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_BINARY_DIR}/generated/Info.plist.in"
    )
endif()

set(EXECUTABLE_NAME "AudioSlicerGuiQt")
if(APPLE AND BUILD_MACOSX_BUNDLE)
    message("${MACOSX_BUNDLE_NAME} / ${EXECUTABLE_NAME}")
    set_target_properties(
        ${TARGET_NAME} PROPERTIES
        OUTPUT_NAME "${MACOSX_BUNDLE_NAME}"
    )
else()
    set_target_properties(
        ${TARGET_NAME} PROPERTIES
        OUTPUT_NAME "${EXECUTABLE_NAME}"
    )
endif()

target_link_libraries(${TARGET_NAME} PUBLIC AudioSlicerLib)
target_include_directories(${TARGET_NAME} PUBLIC "${SLICER_PATH}")
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_BINARY_DIR}/generated/)

find_package(SndFile CONFIG REQUIRED)

set(QT_COMPONENTS Core Widgets Gui)

if(USE_QT5)
    find_package(QT NAMES Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
    find_package(Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
else()
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})
endif()

foreach(item ${QT_COMPONENTS})
    target_link_libraries(${TARGET_NAME} PRIVATE
            Qt${QT_VERSION_MAJOR}::${item})
endforeach()

target_link_libraries(${TARGET_NAME} PRIVATE SndFile::sndfile)  # libsndfile

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(${TARGET_NAME} "stdc++fs")  # std::filesystem (C++17)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES AUTOMOC TRUE)
