cmake_minimum_required(VERSION 2.8.11)

# options
option(ANDROID "switch to android build" ON)
option(TESTS "Enable ARticated tests" OFF)

if(ANDROID)
    # toolchain file/setup
    set( CMAKE_TOOLCHAIN_FILE qt-android-cmake/toolchain/android.toolchain.cmake)
    set( ANDROID_NATIVE_API_LEVEL 23)
else(ANDROID)
endif(ANDROID)

project(articated_app)
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -g -O0")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -O0")

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
#set(SAMPLES_DIR ${CMAKE_SOURCE_DIR}/tests/samples)

include_directories(${INCLUDE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(articated_app)
include(operators)
include(movement3d)
include(movement3d_filter)
include(augmentation_widget)
include(vision)

if(TESTS)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure)
    add_subdirectory(googletest)
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    enable_testing()
    add_subdirectory(tests)
endif(TESTS)
