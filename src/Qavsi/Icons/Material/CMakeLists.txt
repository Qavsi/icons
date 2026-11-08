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
if(NOT BUILD_SHARED_LIBS AND Qt6_VERSION VERSION_LESS 6.10.1)
    # shortcut: QTBUG-140220 skips static plugin initialization in later engines; remove with Qt < 6.10.1 support.
    # A versioned qmldir makes Qt resolve the static plugin by instance rather than by URI.
    qt_query_qml_module(${module_target} VERSION material_version QMLDIR material_qmldir)
    get_filename_component(material_output_dir "${material_qmldir}" DIRECTORY)
    set(compat_dir "${material_output_dir}/qt68")
    function(_qavsi_material_qt68_qmldir directory version)
        file(READ "${directory}/qmldir" content)
        file(WRITE "${directory}/qt68/qmldir" "${content}")
        string(REGEX REPLACE "\nplugin [^\n]+\n" "\n" content "${content}")
        string(APPEND content "import Qavsi.Icons.Material ${version}\n")
        file(WRITE "${directory}/qmldir" "${content}")
    endfunction()
    cmake_language(EVAL CODE
        "cmake_language(DEFER CALL _qavsi_material_qt68_qmldir \"${material_output_dir}\" \"${material_version}\")")
    set_source_files_properties("${compat_dir}/qmldir" PROPERTIES GENERATED TRUE)
    qt_add_resources(${module_target} qt68_material_qmldir
        PREFIX "/qt/qml/Qavsi/Icons/Material.${material_version}" BASE "${compat_dir}"
        FILES "${compat_dir}/qmldir" OUTPUT_TARGETS compat_targets)
    list(APPEND module_auxiliary_targets ${compat_targets})
endif()
target_sources(${module_target}Plugin PRIVATE materialplugin.cpp)
target_link_libraries(${module_target}Plugin PRIVATE QavsiIcons::Core)
target_include_directories(${module_target}Plugin PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
set_target_properties(${module_target} PROPERTIES EXPORT_NAME Material VERSION ${PROJECT_VERSION} SOVERSION 0.2)
set_target_properties(${module_target}Plugin PROPERTIES EXPORT_NAME MaterialPlugin)
target_compile_features(${module_target} PUBLIC cxx_std_20)
target_compile_definitions(${module_target} PRIVATE QAVSI_ICONS_MATERIAL_FONT_QRC_PATH=":${QAVSI_ICONS_MATERIAL_FONT_QRC_PATH}/")
target_link_libraries(${module_target} PUBLIC QavsiIcons::Core PRIVATE Qt6::Quick)
add_library(QavsiIcons::Material ALIAS ${module_target})
include("${PROJECT_SOURCE_DIR}/cmake/InstallQmlModule.cmake")
install(FILES fonts/LICENSE fonts/provenance.json fonts/instances.json
    DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/QavsiIcons/Material")
