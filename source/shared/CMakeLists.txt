# Shared
find_package(Qt6 COMPONENTS Core)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(movement3d)

set(FRAME_DATA_INC frame_data.hpp)
set(FRAME_DATA_SRC )
qt_wrap_cpp(FRAME_DATA_MOC ${FRAME_DATA_INC})

add_library( frame_data STATIC ${FRAME_DATA_SRC} ${FRAME_DATA_INC} ${FRAME_DATA_MOC})
target_link_libraries(frame_data Qt::Core)
