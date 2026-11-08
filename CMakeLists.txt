cmake_minimum_required(VERSION 3.30)
project(QavsiIcons VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(cmake/QtMinimum.cmake)
find_package(Qt6 ${QAVSI_ICONS_QT_MIN} REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES ${QAVSI_ICONS_QT_MIN})
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
option(QAVSI_ICONS_BUILD_MATERIAL "Build the Material Symbols provider" ON)
option(QAVSI_ICONS_BUILD_TESTS "Build icon contract tests" ${PROJECT_IS_TOP_LEVEL})
if(NOT QT_QML_OUTPUT_DIRECTORY)
    set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src")
endif()
add_subdirectory(src/Qavsi/Icons)
if(QAVSI_ICONS_BUILD_MATERIAL)
    add_subdirectory(src/Qavsi/Icons/Material)
endif()
set(QavsiIcons_QML_IMPORT_DIR "${QT_QML_OUTPUT_DIRECTORY}")
if(NOT PROJECT_IS_TOP_LEVEL)
    set(QavsiIcons_QML_IMPORT_DIR "${QavsiIcons_QML_IMPORT_DIR}" PARENT_SCOPE)
endif()
set(qml_install_dir "${CMAKE_INSTALL_LIBDIR}/qml")
configure_package_config_file(cmake/QavsiIconsConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/QavsiIconsConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QavsiIcons"
    PATH_VARS qml_install_dir)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/QavsiIconsConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}" COMPATIBILITY SameMinorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/QavsiIconsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QavsiIconsConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QavsiIcons")
install(EXPORT QavsiIconsCoreTargets NAMESPACE QavsiIcons::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QavsiIcons")
if(QAVSI_ICONS_BUILD_MATERIAL)
    install(EXPORT QavsiIconsMaterialTargets NAMESPACE QavsiIcons::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/QavsiIcons")
endif()
install(FILES LICENSE THIRD_PARTY_NOTICES.md DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/QavsiIcons")
if(QAVSI_ICONS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
