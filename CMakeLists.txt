cmake_minimum_required(VERSION 3.21.1)

project(Mines VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MACOSX_CODESIGN_IDENTITY "" CACHE STRING "Identity to use for Apple code signing.  When present, causes macdeployqt to run against the built bundle.")
mark_as_advanced(MACOSX_CODESIGN_IDENTITY)

find_package(Qt6 REQUIRED COMPONENTS Gui Widgets LinguistTools Svg)

set(TS_FILES Mines_en.ts)

set(PROJECT_SOURCES
        aboutdialog.cpp
        aboutdialog.h
        cell.cpp
        cell.h
        customgamedialog.cpp
        customgamedialog.h
        gameboard.cpp
        gameboard.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        resources/res.qrc
        ${TS_FILES}
)

qt_add_executable(Mines
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Mines APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

qt_add_translations(Mines
    TS_FILES ${TS_FILES}
    SOURCES ${PROJECT_SOURCES}
    RESOURCE_PREFIX /i18n
    NATIVE_TS_FILE Mines_en.ts
    LUPDATE_OPTIONS -source-language en
)

target_link_libraries(Mines PRIVATE Qt6::Widgets Qt6::Svg)

set_target_properties(Mines PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(APPLE)
    get_target_property(_qmake_exe Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_exe}" DIRECTORY)

    find_program(CODESIGN codesign REQUIRED)
    find_program(MACDEPLOYQT macdeployqt REQUIRED HINTS "${_qt_bin_dir}")

    set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/resources/mine.icns")
    set_source_files_properties(
        "${app_icon_macos}"
        PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )

    target_sources(Mines PRIVATE "${app_icon_macos}")

    set_target_properties(
        Mines
        PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_ICON_FILE mine.icns
    )

    if(NOT "${MACOSX_CODESIGN_IDENTITY}" STREQUAL "")
        add_custom_command(
            TARGET Mines
            POST_BUILD
            COMMENT "Deploying Qt and signing the app bundle..."
            COMMAND "${MACDEPLOYQT}"
            ARGS "$<TARGET_BUNDLE_DIR:Mines>" -timestamp -appstore-compliant -codesign=${MACOSX_CODESIGN_IDENTITY} -sign-for-notarization=${MACOSX_CODESIGN_IDENTITY}
            VERBATIM
        )
    endif()
endif()

if(WIN32)
    target_sources(Mines PRIVATE "resources/win.rc")
endif()

include(GNUInstallDirs)
install(TARGETS Mines
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(Mines)
