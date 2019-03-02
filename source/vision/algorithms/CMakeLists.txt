# vision
find_package(Qt5 REQUIRED Core Gui Multimedia OpenGL)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(algorithm_gpu)

set(VISION_INC vision_algorithm.hpp algorithm_original.hpp algorithm_random.hpp)
set(VISION_SRC vision_algorithm.cpp algorithm_original.cpp algorithm_random.cpp)
qt5_wrap_cpp(vision_MOC ${VISION_INC})

add_library(algorithms STATIC ${VISION_SRC} ${VISION_HPP} ${vision_MOC})
target_link_libraries(algorithms ${QT_LIBRARIES} operators movement3d movement3d_filter algorithm_gpu)
target_include_directories(algorithms PUBLIC ${INCLUDE_DIR})

target_compile_options(algorithms PRIVATE -std=c++11 -Wall -Wextra)

qt5_use_modules(algorithms Core Gui Multimedia OpenGL)
