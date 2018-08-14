# movement3d
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(MOVEMENT3D_INC movement3d.hpp)
set(MOVEMENT3D_SRC movement3d.cpp)

add_library( movement3d STATIC ${MOVEMENT3D_SRC} ${MOVEMENT3D_INC} )

target_compile_options(movement3d PRIVATE -std=c++11 -Wall -Wextra)

# movement3d_filter
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(MOVEMENT3D_FILTER_INC movement3d_filter.hpp)
set(MOVEMENT3D_FILTER_SRC movement3d_filter.cpp)

add_library( movement3d_filter STATIC ${MOVEMENT3D_FILTER_SRC} ${MOVEMENT3D_FILTER_INC} )
target_link_libraries(movement3d_filter movement3d )
target_include_directories ( movement3d_filter PUBLIC ${INCLUDE_DIR} )

target_compile_options(movement3d_filter PRIVATE -std=c++11 -Wall -Wextra)
