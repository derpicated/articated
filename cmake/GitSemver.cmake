# Get the Semantic Versioning from git using CMake
#
# The idea was "taken" from:
#   The stlink project (github.com/texane/stlink)
#   See: https://github.com/texane/stlink/blob/master/LICENSE
#
# The following use cases work:
#  - Dirty (sources changed)
#  - No tags in repo (git hash is used)
#  - Tags as: v1.0.0, 1.0.0

include(${CMAKE_CURRENT_LIST_DIR}/StringToSemver.cmake)

# Find the git program
function(find_git)
    find_program(git
        NAMES
            "git" # Linux, Windows
    )
    if(NOT git)
        message(WARNING "Could not find the git executable")
    endif()
endfunction()

# Arguments:
#  MAJOR: Variable name for the major version return value
#  MINOR: Variable name for the minor version return value
#  PATCH: Variable name for the patch version return value
#  STRING: Variable name for the string version return value
#  PATH: Path where the git command is executed
function(git_semver)
    set(options)
    set(oneValueArgs MAJOR MINOR PATCH STRING PATH)
    set(multiValueArgs)
    cmake_parse_arguments(git_semver "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT git_semver_MAJOR)
        message(WARNING "No MAJOR variable specified")
        set(git_semver_MAJOR "__major")
    endif()
    if(NOT git_semver_MINOR)
        message(WARNING "No MINOR variable specified")
        set(git_semver_MINOR "__minor")
    endif()
    if(NOT git_semver_PATCH)
        message(WARNING "No PATCH variable specified")
        set(git_semver_PATCH "__patch")
    endif()
    if(NOT git_semver_STRING)
        message(WARNING "No STRING variable specified")
        set(git_semver_STRING "__string")
    endif()
    if(NOT git_semver_PATH)
        message(WARNING "No PATH variable specified; Using CMAKE_CURRENT_LIST_DIR: (${CMAKE_CURRENT_LIST_DIR})")
        set(git_semver_PATH "${CMAKE_CURRENT_LIST_DIR}")
    endif()

    # Set default values for variables
    set(${git_semver_MAJOR} "0")
    set(${git_semver_MINOR} "0")
    set(${git_semver_PATCH} "0")
    set(${git_semver_STRING} "")

    find_git()
    if(git)
        # Check if HEAD is pointing to a tag
        execute_process (
            COMMAND             "${git}" describe --tags --always
            WORKING_DIRECTORY   "${git_semver_PATH}"
            OUTPUT_VARIABLE     "${git_semver_STRING}"
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        # If the sources have been changed locally, add -dirty to the version.
        execute_process (
            COMMAND             "${git}" diff --quiet
            WORKING_DIRECTORY   "${git_semver_PATH}"
            RESULT_VARIABLE     res)

        if (res EQUAL 1)
            set (${git_semver_STRING} "${${git_semver_STRING}}-dirty")
        endif()

        semver(${${git_semver_STRING}} ${git_semver_MAJOR} ${git_semver_MINOR} ${git_semver_PATCH})
    endif()

    # Apply variables to parent scope
    set(${git_semver_MAJOR} ${${git_semver_MAJOR}} PARENT_SCOPE)
    set(${git_semver_MINOR} ${${git_semver_MINOR}} PARENT_SCOPE)
    set(${git_semver_PATCH} ${${git_semver_PATCH}} PARENT_SCOPE)
    set(${git_semver_STRING} ${${git_semver_STRING}} PARENT_SCOPE)
endfunction()
