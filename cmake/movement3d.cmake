set(MOVEMENT3D_INC
    ${INCLUDE_DIR}/movement3d.hpp
)
set(MOVEMENT3D_SRC
    ${SRC_DIR}/movement3d.cpp
)

add_library( movement3d STATIC ${MOVEMENT3D_SRC} ${MOVEMENT3D_INC} )
target_link_libraries( movement3d operators )
target_include_directories ( movement3d PUBLIC ${INCLUDE_DIR} )

target_compile_options(movement3d PRIVATE -std=c++11 -Wall -Wextra)
