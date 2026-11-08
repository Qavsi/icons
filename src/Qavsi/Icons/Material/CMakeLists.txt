set(module_target QavsiIconsMaterial)
set(module_export QavsiIconsMaterialTargets)
add_library(${module_target})
qt_add_qml_module(${module_target}
    URI Qavsi.Icons.Material
    PLUGIN_TARGET ${module_target}Plugin
    CLASS_NAME MaterialPlugin
    NO_GENERATE_PLUGIN_SOURCE
    NO_PLUGIN_OPTIONAL
    IMPORTS TARGET QavsiIconsCore
    SOURCES materialsymbols.h materialsymbols.cpp
    QML_FILES MaterialGlyph.qml
    RESOURCES fonts/MaterialSymbolsRounded.codepoints
        fonts/instances.json
        fonts/LICENSE fonts/provenance.json
    OUTPUT_TARGETS module_auxiliary_targets)
target_sources(${module_target}Plugin PRIVATE materialplugin.cpp)
target_link_libraries(${module_target}Plugin PRIVATE QavsiIcons::Core)
target_include_directories(${module_target}Plugin PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
set_target_properties(${module_target} PROPERTIES EXPORT_NAME Material VERSION ${PROJECT_VERSION} SOVERSION 0.1)
set_target_properties(${module_target}Plugin PROPERTIES EXPORT_NAME MaterialPlugin)
target_compile_features(${module_target} PUBLIC cxx_std_20)
target_link_libraries(${module_target} PUBLIC QavsiIcons::Core PRIVATE Qt6::Quick)
add_library(QavsiIcons::Material ALIAS ${module_target})
include("${PROJECT_SOURCE_DIR}/cmake/InstallQmlModule.cmake")
install(FILES fonts/LICENSE fonts/provenance.json fonts/instances.json
    DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/QavsiIcons/Material")
