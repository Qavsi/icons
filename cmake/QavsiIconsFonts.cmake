include_guard(GLOBAL)
function(_qavsi_icons_add_font_resources target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "ALL" "QRC_PREFIX" "FONTS;CODEPOINTS")
    if(arg_UNPARSED_ARGUMENTS OR NOT TARGET "${target}" OR NOT arg_QRC_PREFIX OR NOT arg_FONTS
       OR (arg_ALL AND arg_CODEPOINTS) OR (NOT arg_ALL AND NOT arg_CODEPOINTS))
        message(FATAL_ERROR "Invalid font resource arguments for ${target}")
    endif()
    if(EXISTS "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tools/subset-fonts.py")
        set(_QAVSI_FONT_SCRIPT "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tools/subset-fonts.py")
    else()
        set(_QAVSI_FONT_SCRIPT "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/subset-fonts.py")
    endif()
    if(NOT arg_ALL)
        find_package(Python3 REQUIRED COMPONENTS Interpreter)
    endif()
    string(MD5 resource_id "${CMAKE_CURRENT_BINARY_DIR}/${target}/${arg_QRC_PREFIX}")
    set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/qavsi-fonts-${resource_id}")
    foreach(font IN LISTS arg_FONTS)
        get_filename_component(filename "${font}" NAME)
        set(output "${output_dir}/${filename}")
        if(arg_ALL)
            add_custom_command(OUTPUT "${output}"
                COMMAND "${CMAKE_COMMAND}" -E make_directory "${output_dir}"
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${font}" "${output}"
                DEPENDS "${font}" VERBATIM)
        else()
            add_custom_command(OUTPUT "${output}"
                COMMAND "${CMAKE_COMMAND}" -E make_directory "${output_dir}"
                COMMAND "${Python3_EXECUTABLE}" "${_QAVSI_FONT_SCRIPT}"
                    "${font}" "${output}" ${arg_CODEPOINTS}
                DEPENDS "${font}" "${_QAVSI_FONT_SCRIPT}" VERBATIM)
        endif()
        list(APPEND outputs "${output}")
    endforeach()
    qt_add_resources(${target} "qavsi_fonts_${resource_id}" PREFIX "${arg_QRC_PREFIX}"
        BASE "${output_dir}" FILES ${outputs})
endfunction()
