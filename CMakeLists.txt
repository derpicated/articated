cmake_minimum_required(VERSION 3.11)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING
  "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel.")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
  Debug Release RelWithDebInfo MinSizeRel)

# options
option(ANDROID "switch to android build" OFF)
option(TESTS "Enable ARticated tests" OFF)

if(ANDROID)
  set(ANDROID_NATIVE_API_LEVEL "27" CACHE STRING
    "Choose the Android NDK Native APIs version to use.")
else(ANDROID)
endif(ANDROID)

project(articated_app)
include(cmake/GitSemver.cmake)
include_directories(source)
add_subdirectory(source)

if(TESTS)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
    )
    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        message("Downloading googletest")
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
        include_directories(${googletest_SOURCE_DIR}/include ${googletest_SOURCE_DIR})
    endif()

    add_subdirectory(tests)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure)
endif(TESTS)
