#cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_VERBOSE_MAKEFILE OFF)

################################################################################
# Qt library
################################################################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 REQUIRED Core Gui Quick Widgets Multimedia )

################################################################################
# QML resources
################################################################################
file(GLOB_RECURSE articated_app_qml qml/*.qml qml/*.js)
set(articated_app_qml_qrc ${CMAKE_CURRENT_BINARY_DIR}/qml.qrc)
file(WRITE ${articated_app_qml_qrc} "<RCC>\n    <qresource prefix=\"/\">\n")
foreach(qml_file ${articated_app_qml})
    file(RELATIVE_PATH qml_file_relative_path ${CMAKE_CURRENT_BINARY_DIR} ${qml_file})
    file(RELATIVE_PATH qml_file_short_name ${CMAKE_SOURCE_DIR}/qml ${qml_file})
    file(APPEND ${articated_app_qml_qrc} "        <file alias=\"${qml_file_short_name}\">${qml_file_relative_path}</file>\n")
endforeach()
file(APPEND ${articated_app_qml_qrc} "    </qresource>\n</RCC>")

################################################################################
# ARticated app
################################################################################
add_subdirectory(augmentation_widget)
add_subdirectory(movement3d)
add_subdirectory(operators)
add_subdirectory(vision)

qt5_add_resources(articated_app_rcc ${articated_app_qml_qrc}
    ${CMAKE_SOURCE_DIR}/resources/3D_models/3D_models.qrc
    ${CMAKE_SOURCE_DIR}/resources/debug_samples/debug_samples.qrc)
#TODO: delete the following commented lines if functional without
#qt5_add_resources(3D_models_rcc ${CMAKE_SOURCE_DIR}/resources/3D_models/3D_models.qrc)
#qt5_add_resources(3D_models_rcc ${CMAKE_SOURCE_DIR}/resources/debug_samples/debug_samples.qrc)
#qt5_add_resources(3D_models_rcc ${CMAKE_SOURCE_DIR}/resources/GL_shaders/GL_shaders.qrc)


set( articated_app_SOURCES   main.cpp window.cpp)
set( articated_app_HEADERS   window.hpp )
include_directories(AFTER SYSTEM src ${CMAKE_BINARY_DIR})

if(ANDROID)
    add_library(articated_app SHARED ${articated_app_SOURCES} ${articated_app_HEADERS} ${articated_app_rcc} ${articated_app_qml} ${3D_models_rcc})
else()
    add_executable(articated_app ${articated_app_SOURCES} ${articated_app_HEADERS} ${articated_app_rcc} ${articated_app_qml} ${3D_models_rcc})
endif()
target_link_libraries(articated_app vision operators augmentation Qt5::Core Qt5::Gui Qt5::Quick Qt5::Widgets Qt5::Multimedia )

# Get git version
git_semver(
    MAJOR VERSION_MAJOR
    MINOR VERSION_MINOR
    PATCH VERSION_PATCH
    STRING VERSION_STRING
    PATH "${CMAKE_CURRENT_LIST_DIR}"
)
# Set version as a property to the ARticated target
set_target_properties(articated_app PROPERTIES VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

if(ANDROID)
    # Used by the qt-android-cmake project
    set( ANDROID_PLATFORM_LEVEL ${ANDROID_NATIVE_API_LEVEL})
    # Using the LLVM from the latest Android SDK (NDK v19)
    # This is used in the qt-android-cmake project
    set( ANDROID_USE_LLVM TRUE)

    # The STL lib to use
    set(ANDROID_STL_SHARED_LIBRARIES "c++_shared")

    include(../qt-android-cmake/AddQtAndroidApk.cmake)

    # Define the app
    add_qt_android_apk(
        articated_apk
        articated_app
        NAME "ARticated"
        # VERSION_CODE is single number
        # Concatenate the version to single number
        # E.g.
        # 1.0.0 => 100
        # 10.25.1 => 10251
        VERSION_CODE "${VERSION_MAJOR}${VERSION_MINOR}${VERSION_PATCH}"
        PACKAGE_NAME "org.derpicated.articated"
    )
endif()
target_compile_options(articated_app PRIVATE -std=c++11 -Wall -Wextra)

# copy over resource folder
add_custom_command(
        TARGET articated_app PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/res
                ${CMAKE_CURRENT_BINARY_DIR}/package/res)

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/package/AndroidManifest.xml")
    # read manifest
    file(READ ${CMAKE_CURRENT_BINARY_DIR}/package/AndroidManifest.xml ANDROID_MANIFEST_TMP)
    # add icon
    STRING(REGEX REPLACE
            "<application"
            "<application android:icon=\"@mipmap/ic_launcher\""
            ANDROID_MANIFEST_TMP
            ${ANDROID_MANIFEST_TMP})
    # set rotation
    # STRING(REGEX REPLACE
    #         "android:screenOrientation=\"unspecified\""
    #         "android:screenOrientation=\"landscape\""
    #         ANDROID_MANIFEST_TMP
    #         ${ANDROID_MANIFEST_TMP})
    # set theme to fullscreen
    STRING(REGEX REPLACE
            "<activity"
            "<activity android:theme=\"@android:style/Theme.NoTitleBar.Fullscreen\""
            ANDROID_MANIFEST_TMP
            ${ANDROID_MANIFEST_TMP})

    STRING(REGEX REPLACE
            "<uses-permission"
            "<uses-permission android:name=\"android.permission.CAMERA\" />\n <uses-permission"
            ANDROID_MANIFEST_TMP
            ${ANDROID_MANIFEST_TMP})

    # write manifest
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/package/AndroidManifest.xml ${ANDROID_MANIFEST_TMP})
else()
endif()
