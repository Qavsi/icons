set(module_target QavsiIconsCore)
set(module_export QavsiIconsCoreTargets)
qt_add_qml_module(${module_target} SHARED
    URI Qavsi.Icons
    PLUGIN_TARGET ${module_target}Plugin
    SOURCES iconregistry.h iconregistry.cpp nativeiconimage.h nativeiconimage.cpp
    QML_FILES AppIcon.qml IconGlyph.qml
    OUTPUT_TARGETS module_auxiliary_targets)
include(GenerateExportHeader)
generate_export_header(${module_target} EXPORT_FILE_NAME QavsiIconsExport.h)
set_target_properties(${module_target} PROPERTIES EXPORT_NAME Core VERSION ${PROJECT_VERSION} SOVERSION 0.1)
set_target_properties(${module_target}Plugin PROPERTIES EXPORT_NAME CorePlugin)
target_compile_features(${module_target} PUBLIC cxx_std_20)
target_include_directories(${module_target} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Qavsi/Icons>)
target_link_libraries(${module_target} PUBLIC Qt6::Quick)
add_library(QavsiIcons::Core ALIAS ${module_target})
install(FILES iconregistry.h "${CMAKE_CURRENT_BINARY_DIR}/QavsiIconsExport.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/Qavsi/Icons")
include("${PROJECT_SOURCE_DIR}/cmake/InstallQmlModule.cmake")
