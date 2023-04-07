# Shared
find_package(Qt6 REQUIRED COMPONENTS Core)

add_subdirectory(movement3d)

set(CMAKE_AUTOMOC ON)

add_library(frame_data STATIC frame_data.hpp)
target_link_libraries( frame_data Qt::Core)
