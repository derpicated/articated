find_package(Qt6 COMPONENTS Core Multimedia)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Multimedia)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (Qt6_FOUND)
    add_library(qt6_camera_list STATIC qt6_camera_list.cpp)
    target_link_libraries(qt6_camera_list Qt::Core Qt::Multimedia)
    add_library(video_list::lib ALIAS qt6_camera_list)

    add_library(video_sink_acquisition STATIC video_sink_acquisition.cpp)
    target_link_libraries(video_sink_acquisition Qt::Core Qt::Multimedia)
    add_library(acquisition::lib ALIAS video_sink_acquisition)
else ()
    add_library(qt5_camera_list STATIC qt5_camera_list.cpp)
    target_link_libraries(qt5_camera_list Qt::Core Qt::Multimedia)
    add_library(video_list::lib ALIAS qt5_camera_list)

    add_library(video_surface_acquisition STATIC video_surface_acquisition.cpp)
    target_link_libraries(video_surface_acquisition Qt::Core Qt::Multimedia)
    add_library(acquisition::lib ALIAS video_surface_acquisition)
endif ()

target_include_directories(algorithms PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
