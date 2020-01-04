# Shared
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(movement3d)

set(FRAMEDATA_INC framedata.hpp)
set(FRAMEDATA_SRC )

add_library( FRAMEDATA STATIC ${FRAMEDATA_SRC} ${FRAMEDATA_INC} )
