set(APP_ICON_WINDOWS_RC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/windows/appicon.rc" PARENT_SCOPE)
set(APP_ICON_WINDOWS_ICO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/windows/appicon.ico" PARENT_SCOPE)
set(APP_ICON_MACOS_ICNS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/macos/appicon.icns" PARENT_SCOPE)

configure_file(
    app_config.h.in
    ${CMAKE_BINARY_DIR}/generated/app_config.h
)

configure_file(
        macos/Info.plist.in
        ${CMAKE_BINARY_DIR}/generated/Info.plist.in
)
