if(WIN32)
    set(GUI_TYPE WIN32)
elseif(APPLE AND BUILD_MACOSX_BUNDLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif()

add_subdirectory(qt)
