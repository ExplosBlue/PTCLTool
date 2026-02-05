cmake_minimum_required(VERSION 3.16)

project(EffectTool VERSION 0.1 LANGUAGES CXX)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    src/editor/emitterSetWidget.cpp
    src/editor/fluctuationEditorWidget.cpp
    src/editor/ptclListWidget.cpp
    src/editor/stripeEditorWidget.cpp
    src/editor/textureListWidget.cpp
    src/editor/textureImportDialog.cpp
    src/editor/textureSelectDialog.cpp
    src/editor/components/alphaSlider.cpp
    src/editor/components/animGraph.cpp
    src/editor/components/clickableLabel.cpp
    src/editor/components/collapsibleWidget.cpp
    src/editor/components/colorGradientEditor.cpp
    src/editor/components/combinerPreviewWidget.cpp
    src/editor/components/gradientSlider.cpp
    src/editor/components/loadingSpinner.cpp
    src/editor/components/rgbaColorWidget.cpp
    src/editor/components/thumbnailWidget.cpp
    src/editor/childEditor/alphaPropertiesWidget.cpp
    src/editor/childEditor/basicPropertiesWidget.cpp
    src/editor/childEditor/childEditorWidget.cpp
    src/editor/childEditor/colorPropertiesWidget.cpp
    src/editor/childEditor/combinerPropertiesWidget.cpp
    src/editor/childEditor/emissionPropertiesWidget.cpp
    src/editor/childEditor/rotationPropertiesWidget.cpp
    src/editor/childEditor/scalePropertiesWidget.cpp
    src/editor/childEditor/texturePropertiesWidget.cpp
    src/editor/childEditor/velocityPropertiesWidget.cpp
    src/editor/emitterWidget/alphaPropertiesWidget.cpp
    src/editor/emitterWidget/basicPropertiesWidget.cpp
    src/editor/emitterWidget/colorPropertiesWidget.cpp
    src/editor/emitterWidget/combinerPropertiesWidget.cpp
    src/editor/emitterWidget/emissionPropertiesWidget.cpp
    src/editor/emitterWidget/emitterWidget.cpp
    src/editor/emitterWidget/gravityPropertiesWidget.cpp
    src/editor/emitterWidget/lifespanPropertiesWidget.cpp
    src/editor/emitterWidget/rotationPropertiesWidget.cpp
    src/editor/emitterWidget/scalePropertiesWidget.cpp
    src/editor/emitterWidget/terminationPropertiesWidget.cpp
    src/editor/emitterWidget/texturePropertiesWidget.cpp
    src/editor/emitterWidget/transformPropertiesWidget.cpp
    src/editor/emitterWidget/velocityPropertiesWidget.cpp
    src/editor/emitterWidget/volumePropertiesWidget.cpp
    src/editor/fieldEditor/collisionDataWidget.cpp
    src/editor/fieldEditor/convergenceDataWidget.cpp
    src/editor/fieldEditor/fieldEditorWidget.cpp
    src/editor/fieldEditor/magnetDataWidget.cpp
    src/editor/fieldEditor/posAddDataWidget.cpp
    src/editor/fieldEditor/randomDataWidget.cpp
    src/editor/fieldEditor/spinDataWidget.cpp
    src/etc1/rg_etc1.cpp
    src/ptcl/ptcl.cpp
    src/ptcl/ptclBinary.cpp
    src/ptcl/ptclChildData.cpp
    src/ptcl/ptclEmitter.cpp
    src/ptcl/ptclEmitterSet.cpp
    src/ptcl/ptclFieldData.cpp
    src/ptcl/ptclSeed.cpp
    src/ptcl/ptclTexture.cpp
    src/util/imageUtil.cpp
    src/util/nameValidator.cpp
    src/util/settingsUtil.cpp
)

set(PROJECT_HEADERS
    include/typedefs.h
    include/editor/mainWindow.h
    include/editor/emitterSetWidget.h
    include/editor/fluctuationEditorWidget.h
    include/editor/ptclListWidget.h
    include/editor/stripeEditorWidget.h
    include/editor/textureImportDialog.h
    include/editor/textureListWidget.h
    include/editor/textureSelectDialog.h
    include/editor/components/alphaSlider.h
    include/editor/components/animGraph.h
    include/editor/components/clickableLabel.h
    include/editor/components/collapsibleWidget.h
    include/editor/components/colorGradientEditor.h
    include/editor/components/combinerPreviewWidget.h
    include/editor/components/enumComboBox.h
    include/editor/components/gradientSlider.h
    include/editor/components/loadingSpinner.h
    include/editor/components/rgbaColorWidget.h
    include/editor/components/sizedSpinBox.h
    include/editor/components/thumbnailWidget.h
    include/editor/components/vectorSpinBox.h
    include/editor/childEditor/alphaPropertiesWidget.h
    include/editor/childEditor/basicPropertiesWidget.h
    include/editor/childEditor/childEditorWidget.h
    include/editor/childEditor/colorPropertiesWidget.h
    include/editor/childEditor/combinerPropertiesWidget.h
    include/editor/childEditor/emissionPropertiesWidget.h
    include/editor/childEditor/rotationPropertiesWidget.h
    include/editor/childEditor/scalePropertiesWidget.h
    include/editor/childEditor/texturePropertiesWidget.h
    include/editor/childEditor/velocityPropertiesWidget.h
    include/editor/emitterWidget/alphaPropertiesWidget.h
    include/editor/emitterWidget/basicPropertiesWidget.h
    include/editor/emitterWidget/colorPropertiesWidget.h
    include/editor/emitterWidget/combinerPropertiesWidget.h
    include/editor/emitterWidget/emissionPropertiesWidget.h
    include/editor/emitterWidget/emitterWidget.h
    include/editor/emitterWidget/gravityPropertiesWidget.h
    include/editor/emitterWidget/lifespanPropertiesWidget.h
    include/editor/emitterWidget/rotationPropertiesWidget.h
    include/editor/emitterWidget/scalePropertiesWidget.h
    include/editor/emitterWidget/terminationPropertiesWidget.h
    include/editor/emitterWidget/texturePropertiesWidget.h
    include/editor/emitterWidget/transformPropertiesWidget.h
    include/editor/emitterWidget/velocityPropertiesWidget.h
    include/editor/emitterWidget/volumePropertiesWidget.h
    include/editor/fieldEditor/collisionDataWidget.h
    include/editor/fieldEditor/convergenceDataWidget.h
    include/editor/fieldEditor/fieldEditorWidget.h
    include/editor/fieldEditor/magnetDataWidget.h
    include/editor/fieldEditor/posAddDataWidget.h
    include/editor/fieldEditor/randomDataWidget.h
    include/editor/fieldEditor/spinDataWidget.h
    include/etc1/rg_etc1.h
    include/math/matrix.h
    include/math/util.h
    include/math/vector.h
    include/ptcl/ptcl.h
    include/ptcl/ptclBinary.h
    include/ptcl/ptclChildData.h
    include/ptcl/ptclEmitter.h
    include/ptcl/ptclEmitterSet.h
    include/ptcl/ptclEnum.h
    include/ptcl/ptclFieldData.h
    include/ptcl/ptclFluctuationData.h
    include/ptcl/ptclSeed.h
    include/ptcl/ptclStripeData.h
    include/ptcl/ptclTexture.h
    include/util/bitflagUtil.h
    include/util/imageUtil.h
    include/util/nameValidator.h
    include/util/printUtil.h
    include/util/settingsUtil.h
    include/util/stringUtil.h
)

qt_add_executable(EffectTool
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES} ${PROJECT_HEADERS}
    include/util/randomUtil.h
)

qt_add_resources(EffectTool "icons"
    PREFIX "/"
    FILES
        res/icons/add_emitter.png
        res/icons/add_emitterset.png
        res/icons/copy.png
        res/icons/emitter.png
        res/icons/emitterset.png
        res/icons/emitterset_expanded.png
        res/icons/filter.png
        res/icons/paste.png
        res/icons/remove.png
)

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

target_link_libraries(EffectTool PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Iconv::Iconv Qt${QT_VERSION_MAJOR}::Concurrent)

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

if(WIN32)
    add_custom_command(TARGET EffectTool POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/third_party/iconv/bin/iconv.dll"
            $<TARGET_FILE_DIR:EffectTool>)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EffectTool)
endif()
