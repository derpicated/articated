# vision
find_package(Qt5 REQUIRED Core Multimedia OpenGL)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(shared)
add_subdirectory(original)
add_subdirectory(random)
add_subdirectory(gpu)

set(VISION_INC vision_algorithm.hpp)
set(VISION_SRC vision_algorithm.cpp)
qt5_wrap_cpp(vision_MOC ${VISION_INC})

add_library(algorithms STATIC ${VISION_SRC} ${VISION_HPP} ${vision_MOC})
target_link_libraries(algorithms ${QT_LIBRARIES} framedata movement3d movement3d_filter
  algorithm_shared algorithm_original algorithm_random algorithm_gpu)
target_include_directories(algorithms PUBLIC ${INCLUDE_DIR})

qt5_use_modules(algorithms Core Multimedia OpenGL)
