find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia OpenGL)

set(CMAKE_AUTOMOC ON)

add_subdirectory(algorithms)

add_library(vision STATIC vision.cpp)
target_link_libraries(vision
        Qt::Core
        Qt::Gui
        Qt::Multimedia
        Qt::OpenGL
        frame_data
        algorithms
        movement3d
        movement3d_filter
        )
target_include_directories(vision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
