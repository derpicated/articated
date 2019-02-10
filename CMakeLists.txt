cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING
  "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel.")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
  Debug Release RelWithDebInfo MinSizeRel)

# options
option(ANDROID "switch to android build" OFF)
option(TESTS "Enable ARticated tests" OFF)

if(ANDROID)
    # toolchain file/setup
    set( CMAKE_TOOLCHAIN_FILE qt-android-cmake/toolchain/android.toolchain.cmake)
    set( ANDROID_NATIVE_API_LEVEL 23)
else(ANDROID)
endif(ANDROID)

project(articated_app)
include_directories(source)
add_subdirectory(source)

if(TESTS)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure)
    add_subdirectory(googletest)
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    enable_testing()
    add_subdirectory(tests)
endif(TESTS)
