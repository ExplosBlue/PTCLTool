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
    src/imageUtil.cpp
    src/editor/mainWindow.cpp
    src/editor/emitterWidget.cpp
    src/editor/emitterSetWidget.cpp
    src/editor/textureWidget.cpp
    src/editor/ptclListWidget.cpp
    src/ptcl/ptcl.cpp
    src/ptcl/ptclBinary.cpp
    src/ptcl/ptclChildData.cpp
    src/ptcl/ptclEmitter.cpp
    src/ptcl/ptclEmitterSet.cpp
    src/ptcl/ptclEnum.cpp
    src/ptcl/ptclFieldData.cpp
    src/ptcl/ptclFluctuationData.cpp
    src/ptcl/ptclStripeData.cpp
    src/ptcl/ptclTexture.cpp
    src/etc1/rg_etc1.cpp
)

set(PROJECT_HEADERS
    include/printUtil.h
    include/typedefs.h
    include/imageUtil.h
    include/bitflagUtil.h
    include/editor/mainWindow.h
    include/editor/emitterWidget.h
    include/editor/emitterSetWidget.h
    include/editor/textureWidget.h
    include/editor/ptclListWidget.h
    include/editor/sizedSpinBox.h
    include/editor/vectorSpinBox.h
    include/ptcl/ptcl.h
    include/ptcl/ptclBinary.h
    include/ptcl/ptclChildData.h
    include/ptcl/ptclEmitter.h
    include/ptcl/ptclEmitterSet.h
    include/ptcl/ptclEnum.h
    include/ptcl/ptclFieldData.h
    include/ptcl/ptclFluctuationData.h
    include/ptcl/ptclStripeData.h
    include/ptcl/ptclTexture.h
    include/etc1/rg_etc1.h
)

set(PROJECT_FORMS
    src/editor/ui/mainwindow.ui
)

qt_add_executable(EffectTool
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_FORMS}
)

qt_add_resources(EffectTool "icons"
    PREFIX "/"
    FILES
        res/icons/remove.png
)

add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
add_link_options(-fsanitize=address)
add_definitions(-DADDRESS_SANITIZER)

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
