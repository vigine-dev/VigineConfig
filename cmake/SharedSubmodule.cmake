# A third-party tree this repository pins is read from ${VIGINE_SHARED_ROOT}/<name> when that folder
# exists, so a tree that already holds the project does not get a second copy; else external/<name>.

set(VIGINE_SHARED_ROOT "" CACHE PATH "The folder that holds the shared checkouts of the projects (e.g. ~/vigine)")

function(vigineconfig_shared_tree name out_var)
    set(chosen "${CMAKE_CURRENT_SOURCE_DIR}/external/${name}")
    if(NOT "${VIGINE_SHARED_ROOT}" STREQUAL "" AND IS_DIRECTORY "${VIGINE_SHARED_ROOT}/${name}")
        set(chosen "${VIGINE_SHARED_ROOT}/${name}")
    endif()
    message(STATUS "${name}: tree at ${chosen}")
    set(${out_var} "${chosen}" PARENT_SCOPE)
endfunction()
