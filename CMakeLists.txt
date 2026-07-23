cmake_minimum_required(VERSION 3.16)

project(PTCLTool VERSION 1.0.0 LANGUAGES CXX)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent)

find_package(Iconv QUIET)
if(NOT Iconv_FOUND)
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third_party/iconv/lib/cmake")
    find_package(Iconv REQUIRED CONFIG)
endif()

set(PROJECT_SOURCES
    src/main.cpp
    src/editor/mainWindow.cpp
    src/editor/ptclListWidget.cpp
    src/editor/textureListWidget.cpp
    src/editor/textureImportDialog.cpp
    src/editor/textureSelectDialog.cpp
    src/editor/components/animGraph.cpp
    src/editor/components/clickableLabel.cpp
    src/editor/components/collapsibleWidget.cpp
    src/editor/components/colorChannelSlider.cpp
    src/editor/components/colorGradientEditor.cpp
    src/editor/components/combinerPreviewWidget.cpp
    src/editor/components/frameSelectorPopup.cpp
    src/editor/components/frameTimeline.cpp
    src/editor/components/loadingSpinner.cpp
    src/editor/components/rgbaColorWidget.cpp
    src/editor/components/startframelist.cpp
    src/editor/components/texturePreview.cpp
    src/editor/components/thumbnailWidget.cpp
    src/editor/components/viewportScrollBar.cpp
    src/editor/inspector/colorInspector.cpp
    src/editor/inspector/combinerInspector.cpp
    src/editor/inspector/emitterSetInspector.cpp
    src/editor/inspector/generalEmitterInspector.cpp
    src/editor/inspector/inspectorPanel.cpp
    src/editor/inspector/inspectorWidgetBase.cpp
    src/editor/inspector/fluctuationInspector.cpp
    src/editor/inspector/lifespanInspector.cpp
    src/editor/inspector/physicsInspector.cpp
    src/editor/inspector/rotationScaleInspector.cpp
    src/editor/inspector/textureInspector.cpp
    src/editor/inspector/shapeInspector.cpp
    src/editor/inspector/child/childColorInspector.cpp
    src/editor/inspector/child/childCombinerInspector.cpp
    src/editor/inspector/child/childEmissionInspector.cpp
    src/editor/inspector/child/childGeneralInspector.cpp
    src/editor/inspector/child/childRotationScaleInspector.cpp
    src/editor/inspector/child/childTextureInspector.cpp
    src/editor/inspector/child/childVelocityInspector.cpp
    src/editor/inspector/field/fieldCollisionInspector.cpp
    src/editor/inspector/field/fieldConvergenceInspector.cpp
    src/editor/inspector/field/fieldMagnetInspector.cpp
    src/editor/inspector/field/fieldPosAddInspector.cpp
    src/editor/inspector/field/fieldRandomInspector.cpp
    src/editor/inspector/field/fieldSpinInspector.cpp
    src/etc1/rg_etc1.cpp
    src/gfx/color.cpp
    src/ptcl/ptcl.cpp
    src/ptcl/ptclBinary.cpp
    src/ptcl/ptclDocument.cpp
    src/ptcl/ptclEmitter.cpp
    src/ptcl/ptclEmitterSet.cpp
    src/ptcl/ptclSeed.cpp
    src/ptcl/ptclTexture.cpp
    src/util/imageUtil.cpp
    src/util/nameValidator.cpp
    src/util/paintUtil.cpp
    src/util/settingsUtil.cpp
)

set(PROJECT_HEADERS
    include/typedefs.h
    include/editor/mainWindow.h
    include/editor/ptclListWidget.h
    include/editor/textureImportDialog.h
    include/editor/textureListWidget.h
    include/editor/textureSelectDialog.h
    include/editor/components/animGraph.h
    include/editor/components/clickableLabel.h
    include/editor/components/collapsibleWidget.h
    include/editor/components/colorChannelSlider.h
    include/editor/components/colorGradientEditor.h
    include/editor/components/combinerPreviewWidget.h
    include/editor/components/enumComboBox.h
    include/editor/components/frameSelectorPopup.h
    include/editor/components/frameTimeline.h
    include/editor/components/loadingSpinner.h
    include/editor/components/rgbaColorWidget.h
    include/editor/components/sizedSpinBox.h
    include/editor/components/startframelist.h
    include/editor/components/texturePreview.h
    include/editor/components/thumbnailWidget.h
    include/editor/components/vectorSpinBox.h
    include/editor/components/viewportScrollBar.h
    include/editor/inspector/colorInspector.h
    include/editor/inspector/combinerInspector.h
    include/editor/inspector/emitterSetInspector.h
    include/editor/inspector/generalEmitterInspector.h
    include/editor/inspector/inspectorPanel.h
    include/editor/inspector/inspectorWidgetBase.h
    include/editor/inspector/fluctuationInspector.h
    include/editor/inspector/lifespanInspector.h
    include/editor/inspector/physicsInspector.h
    include/editor/inspector/rotationScaleInspector.h
    include/editor/inspector/textureInspector.h
    include/editor/inspector/shapeInspector.h
    include/editor/inspector/child/childColorInspector.h
    include/editor/inspector/child/childEmissionInspector.h
    include/editor/inspector/child/childGeneralInspector.h
    include/editor/inspector/child/childCombinerInspector.h
    include/editor/inspector/child/childRotationScaleInspector.h
    include/editor/inspector/child/childTextureInspector.h
    include/editor/inspector/child/childVelocityInspector.h
    include/editor/inspector/field/fieldCollisionInspector.h
    include/editor/inspector/field/fieldConvergenceInspector.h
    include/editor/inspector/field/fieldMagnetInspector.h
    include/editor/inspector/field/fieldPosAddInspector.h
    include/editor/inspector/field/fieldRandomInspector.h
    include/editor/inspector/field/fieldSpinInspector.h
    include/etc1/rg_etc1.h
    include/gfx/color.h
    include/math/matrix.h
    include/math/util.h
    include/math/vector.h
    include/ptcl/ptcl.h
    include/ptcl/ptclBinary.h
    include/ptcl/ptclCommand.h
    include/ptcl/ptclDocument.h
    include/ptcl/ptclEmitter.h
    include/ptcl/ptclEmitterSet.h
    include/ptcl/ptclEnum.h
    include/ptcl/ptclSeed.h
    include/ptcl/ptclTexture.h
    include/util/bitflagUtil.h
    include/util/imageUtil.h
    include/util/nameValidator.h
    include/util/paintUtil.h
    include/util/printUtil.h
    include/util/randomUtil.h
    include/util/settingsUtil.h
    include/util/stringUtil.h
)

qt_add_executable(PTCLTool
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES} ${PROJECT_HEADERS}
)

qt_add_resources(PTCLTool "icons"
    PREFIX "/"
    FILES
        res/icons/add_emitter.png
        res/icons/add_emitterset.png
        res/icons/copy.png
        res/icons/delete_image.png
        res/icons/emitter.png
        res/icons/emitterset.png
        res/icons/emitterset_expanded.png
        res/icons/export_image.png
        res/icons/filter.png
        res/icons/import_image.png
        res/icons/open_file.png
        res/icons/paste.png
        res/icons/recent.png
        res/icons/remove.png
        res/icons/replace_image.png
        res/icons/save.png
        res/icons/save_as.png
)

if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")
    file(WRITE "${APP_ICON_RESOURCE}" "IDI_ICON1 ICON \"${CMAKE_SOURCE_DIR}/res/icons/app_icon.ico\"\n")
    set_source_files_properties(${APP_ICON_RESOURCE} PROPERTIES HEADER_FILE_ONLY TRUE)
    target_sources(PTCLTool PRIVATE "${APP_ICON_RESOURCE}")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(src/etc1/rg_etc1.cpp PROPERTIES
        COMPILE_FLAGS "-Wno-pedantic -Wno-class-memaccess -Wno-type-limits"
    )
endif()

if(APPLE)
    set_target_properties(PTCLTool PROPERTIES
        MACOSX_BUNDLE_ICON_FILE app_icon.icns
    )
    qt_add_resources(PTCLTool "mac_icons" PREFIX "/" FILES res/icons/app_icon.icns)
endif()

if(ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        add_compile_options(/fsanitize=address /Zi)
        add_link_options(/fsanitize=address /INCREMENTAL:NO)
    else()
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
    add_definitions(-DADDRESS_SANITIZER)
endif()

target_link_libraries(PTCLTool PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Iconv::Iconv Qt${QT_VERSION_MAJOR}::Concurrent)

target_include_directories(PTCLTool PRIVATE include)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.ExplosBlue.PTCLTool)
endif()
set_target_properties(PTCLTool PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PTCLTool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(LINUX)
    install(FILES res/icons/app_icon.png
        DESTINATION share/icons/hicolor/256x256/apps
        RENAME org.ExplosBlue.PTCLTool.png
    )
    install(FILES res/org.ExplosBlue.PTCLTool.desktop
        DESTINATION share/applications
    )
endif()

if(WIN32)
    add_custom_command(TARGET PTCLTool POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/third_party/iconv/bin/iconv.dll"
            $<TARGET_FILE_DIR:PTCLTool>)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PTCLTool)
endif()
