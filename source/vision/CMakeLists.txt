find_package(Qt6 COMPONENTS Core Gui Multimedia OpenGL)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Multimedia OpenGL)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(algorithms)
add_subdirectory(acquisition)

add_library(vision STATIC vision.cpp)
target_link_libraries(
    vision
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    Qt::OpenGL
    frame_data
    algorithms
    movement3d
    movement3d_filter
    acquisition::lib
)
target_include_directories(vision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
