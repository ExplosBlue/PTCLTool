cmake_minimum_required(VERSION 3.16)

project(EffectTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/imageutil.cpp
        src/emitterwidget.cpp
        src/emitterSetWidget.cpp
        src/ptclList.cpp
        src/ptcl/ptcl.cpp
)

set(PROJECT_HEADERS
        include/gr.h
        include/mainwindow.h
        include/printUtil.h
        include/typedefs.h
        include/imageutil.h
        include/emitterwidget.h
        include/emitterSetWidget.h
        include/ptclList.h
        include/ptcl/ptcl.h
        include/ptcl/ptclEnums.h
        include/ptcl/ptclRes.h
)

set(PROJECT_FORMS
        ui/mainwindow.ui
)

qt_add_executable(EffectTool
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_FORMS}
)

target_link_libraries(EffectTool PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(EffectTool PRIVATE include)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.EffectTool)
endif()
set_target_properties(EffectTool PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS EffectTool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EffectTool)
endif()
