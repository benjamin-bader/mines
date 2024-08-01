cmake_minimum_required(VERSION 3.21.1)

project(Mines VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Gui Widgets LinguistTools)

set(TS_FILES Mines_en.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        ${TS_FILES}
)

qt_add_executable(Mines
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    cell.h cell.cpp
    gameboard.h gameboard.cpp
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

target_link_libraries(Mines PRIVATE Qt6::Widgets)

set_target_properties(Mines PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (APPLE)
    target_compile_options(Mines PRIVATE -fexperimental-library)
endif()

include(GNUInstallDirs)
install(TARGETS Mines
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(Mines)
