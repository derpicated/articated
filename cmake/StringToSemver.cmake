# Get the Semantic Version from a string in to form of:
#  - x1.2.3
#  - 1.2.3
#

# v_string_: The string where the version needs to be extracted
# v_major_: Variable name for the major version
# v_minor_: Variable name for the minor version
# v_patch_: Variable name for the patch version
function(semver v_string_ v_major_ v_minor_ v_patch_)
    # Set default values for variables
    set(${v_major_} "0")
    set(${v_minor_} "0")
    set(${v_patch_} "0")

    # Get major, minor and patch versions
    string(
        REGEX REPLACE
        "(0|[1-9][0-9]*)[.](0|[1-9][0-9]*)[.](0|[1-9][0-9]*)(-[.0-9A-Za-z-]+)?([+][.0-9A-Za-z-]+)?$"
        "\\1;\\2;\\3"
        match_version_list_
        ${v_string_}
    )
    list(LENGTH match_version_list_ len)
    if(len EQUAL 3)
        list(GET match_version_list_ 0 ${v_major_})
        list(GET match_version_list_ 1 ${v_minor_})
        list(GET match_version_list_ 2 ${v_patch_})
    endif()
    # Drop character(s) before the version number of major
    string(
        REGEX REPLACE
        "^[^[1-9]]*([1-9]+)"
        "\\1;"
        match_version_list_
        ${${v_major_}}
    )
    list(GET match_version_list_ 0 ${v_major_})

    # Apply variables to parent scope
    set(${v_major_} ${${v_major_}} PARENT_SCOPE)
    set(${v_minor_} ${${v_minor_}} PARENT_SCOPE)
    set(${v_patch_} ${${v_patch_}} PARENT_SCOPE)
    set(${v_string_} ${${v_string_}} PARENT_SCOPE)
endfunction()
