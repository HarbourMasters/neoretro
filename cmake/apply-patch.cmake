find_package(Git REQUIRED)

execute_process(
    COMMAND ${GIT_EXECUTABLE} apply --reverse --check --ignore-whitespace ${PATCH}
    RESULT_VARIABLE ALREADY_APPLIED
    OUTPUT_QUIET ERROR_QUIET
)
if (ALREADY_APPLIED EQUAL 0)
    message(STATUS "Patch already applied: ${PATCH}")
    return()
endif()

execute_process(
    COMMAND ${GIT_EXECUTABLE} apply --ignore-whitespace ${PATCH}
    RESULT_VARIABLE APPLY_RESULT
)
if (NOT APPLY_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to apply ${PATCH}")
endif()
message(STATUS "Applied patch: ${PATCH}")
