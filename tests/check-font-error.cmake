execute_process(COMMAND "${CMAKE_COMMAND}" -S "${SOURCE}" -B "${BINARY}"
    "-DQAVSI_SOURCE=${QAVSI_SOURCE}" "-DCASE=${CASE}"
    RESULT_VARIABLE result OUTPUT_VARIABLE out ERROR_VARIABLE err)
if(CASE STREQUAL "duplicate")
    set(expected "only once")
else()
    set(expected "final executable")
endif()
string(CONCAT output "${out}" "${err}")
if(result EQUAL 0 OR NOT output MATCHES "${expected}")
    message(FATAL_ERROR "Expected ${expected}; result=${result}: ${output}")
endif()
