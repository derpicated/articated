# Shared
find_package(Qt5 REQUIRED Core)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(movement3d)

set(FRAME_DATA_INC frame_data.hpp)
set(FRAME_DATA_SRC )
qt5_wrap_cpp(FRAME_DATA_MOC ${FRAME_DATA_INC})

add_library( frame_data STATIC ${FRAME_DATA_SRC} ${FRAME_DATA_INC} ${FRAME_DATA_MOC})
target_link_libraries( frame_data ${QT_LIBRARIES} )

qt5_use_modules( frame_data Core )
