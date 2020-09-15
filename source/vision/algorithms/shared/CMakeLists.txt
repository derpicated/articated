# algorithm shared
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(ALGORITHM_SHARED_INC operators.hpp classification.hpp image.hpp point.hpp)
set(ALGORITHM_SHARED_SRC operators.cpp classification.cpp)
qt5_wrap_cpp(vision_MOC ${VISION_INC})

add_library(algorithm_shared STATIC ${ALGORITHM_SHARED_SRC} ${ALGORITHM_SHARED_INC})
target_link_libraries(algorithm_shared ${QT_LIBRARIES} movement3d movement3d_filter frame_data)
target_include_directories(algorithm_shared PUBLIC ${INCLUDE_DIR})
