project(AudioSlicerGuiQt)

set(TARGET_NAME ${PROJECT_NAME})
add_executable(${TARGET_NAME} ${GUI_TYPE}
main_qt.cpp mainwindow.cpp mainwindow.h mainwindow.cpp workthread.cpp workthread.h)

set_target_properties(
    ${TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${TARGET_NAME} PUBLIC AudioSlicerLib)
target_include_directories(${TARGET_NAME} PUBLIC "${SLICER_PATH}")

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

#target_link_libraries(${TARGET_NAME} PRIVATE
#        Qt${QT_VERSION_MAJOR}::Core
#        Qt${QT_VERSION_MAJOR}::Widgets
#        Qt${QT_VERSION_MAJOR}::Gui)

target_link_libraries(${TARGET_NAME} PRIVATE SndFile::sndfile)  # libsndfile

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(${TARGET_NAME} "stdc++fs")  # std::filesystem (C++17)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES AUTOMOC TRUE)

if(APPLE AND BUILD_MACOSX_BUNDLE)
    set_target_properties(${TARGET_NAME} PROPERTIES
        BUILD_RPATH "@executable_path/../Frameworks"
        INSTALL_RPATH "@executable_path/../Frameworks"    
    )
endif()
