set(MOVEMENT3D_FILTER_INC
    ${INCLUDE_DIR}/movement3d_filter.hpp
)
set(MOVEMENT3D_FILTER_SRC
    ${SRC_DIR}/movement3d_filter.cpp
)

add_library( movement3d_filter STATIC ${MOVEMENT3D_FILTER_SRC} ${MOVEMENT3D_FILTER_INC} )
target_link_libraries(movement3d_filter movement3d )
target_include_directories ( movement3d_filter PUBLIC ${INCLUDE_DIR} )

target_compile_options(movement3d_filter PRIVATE -std=c++11 -Wall -Wextra)
