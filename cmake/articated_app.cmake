#cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_VERBOSE_MAKEFILE OFF)

################################################################################
# take Qt from the QTDIR environment variable
################################################################################
if(DEFINED ENV{QTDIR})
    set(CMAKE_PREFIX_PATH $ENV{QTDIR} ${CMAKE_PREFIX_PATH})
else()
    message(FATAL_ERROR "ERROR: Environment variable QTDIR is not set. Please locate your Qt folder MY_QT5_DIR.")
endif()

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
# App hello
################################################################################
qt5_add_resources(articated_app_rcc ${articated_app_qml_qrc})
qt5_add_resources(3D_models_rcc ${CMAKE_SOURCE_DIR}/3D_models/3D_models.qrc)
set( articated_app_SOURCES   ${SRC_DIR}/main.cpp ${SRC_DIR}/window.cpp)
set( articated_app_HEADERS   ${INCLUDE_DIR}/window.h )
include_directories(AFTER SYSTEM src ${CMAKE_BINARY_DIR})

include(augmentation_widget)

if(ANDROID)
    add_library(articated_app SHARED ${articated_app_SOURCES} ${articated_app_HEADERS} ${articated_app_rcc} ${articated_app_qml} ${3D_models_rcc})
else()
    add_executable(articated_app ${articated_app_SOURCES} ${articated_app_HEADERS} ${articated_app_rcc} ${articated_app_qml} ${3D_models_rcc})
endif()
target_link_libraries(articated_app Qt5::Core Qt5::Gui Qt5::Quick Qt5::Widgets Qt5::Multimedia augmentation)

if(ANDROID)
    include(qt-android-cmake/AddQtAndroidApk.cmake)
    add_qt_android_apk(
        articated_apk
        articated_app
        NAME "ARticated"
        VERSION_CODE 1
        PACKAGE_NAME "org.derpicated.articated_app"
    )
endif()
target_compile_options(articated_app PRIVATE -std=c++11 -Wall -Wextra)

# copy over resource folder
add_custom_command(
        TARGET articated_app PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/res
                ${CMAKE_BINARY_DIR}/res)



if(EXISTS "${CMAKE_BINARY_DIR}/package/AndroidManifest.xml")
    # read manifest
    file(READ ${CMAKE_BINARY_DIR}/package/AndroidManifest.xml ANDROID_MANIFEST_TMP)
    # add icon
    STRING(REGEX REPLACE
            "<application"
            "<application android:icon=\"@mipmap/ic_launcher\""
            ANDROID_MANIFEST_TMP
            ${ANDROID_MANIFEST_TMP})
    # write manifest
    file(WRITE ${CMAKE_BINARY_DIR}/package/AndroidManifest.xml ${ANDROID_MANIFEST_TMP})
else()
endif()
