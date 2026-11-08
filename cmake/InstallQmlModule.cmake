qt_query_qml_module(${module_target}
    TARGET_PATH module_path QMLDIR module_qmldir TYPEINFO module_typeinfo
    QML_FILES module_files QML_FILES_DEPLOY_PATHS module_file_paths)
set(module_install_dir "${CMAKE_INSTALL_LIBDIR}/qml/${module_path}")
file(RELATIVE_PATH module_to_lib "/${module_install_dir}" "/${CMAKE_INSTALL_LIBDIR}")
if(APPLE)
    set_target_properties(${module_target}Plugin PROPERTIES INSTALL_RPATH "@loader_path/${module_to_lib}")
elseif(UNIX)
    set_target_properties(${module_target}Plugin PROPERTIES INSTALL_RPATH "$ORIGIN/${module_to_lib}")
endif()
install(TARGETS ${module_target} ${module_auxiliary_targets} EXPORT ${module_export}
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    OBJECTS DESTINATION "${CMAKE_INSTALL_LIBDIR}/qavsi-icons/objects")
install(TARGETS ${module_target}Plugin EXPORT ${module_export}
    RUNTIME DESTINATION "${module_install_dir}"
    LIBRARY DESTINATION "${module_install_dir}")
install(FILES "${module_qmldir}" "${module_typeinfo}" DESTINATION "${module_install_dir}")
foreach(source destination IN ZIP_LISTS module_files module_file_paths)
    get_filename_component(directory "${destination}" DIRECTORY)
    install(FILES "${source}" DESTINATION "${module_install_dir}/${directory}")
endforeach()
