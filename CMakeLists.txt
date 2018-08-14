cmake_minimum_required(VERSION 2.8.11)

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
