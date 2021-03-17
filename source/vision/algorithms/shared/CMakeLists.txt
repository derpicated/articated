find_package(Qt5 REQUIRED Core Multimedia OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(ALGORITHM_SHARED_INC operators.hpp classification.hpp frame_helper.hpp image.hpp point.hpp algorithm_interface.hpp)
set(ALGORITHM_SHARED_SRC operators.cpp classification.cpp frame_helper.cpp)
qt5_wrap_cpp(vision_MOC ${VISION_INC})

add_library(algorithm_shared STATIC ${ALGORITHM_SHARED_SRC} ${ALGORITHM_SHARED_INC})
target_link_libraries(algorithm_shared movement3d movement3d_filter frame_data ${QT_LIBRARIES} $<$<BOOL:${ANDROID}>:GLESv3> )
target_include_directories(algorithm_shared PUBLIC ${INCLUDE_DIR})

qt5_use_modules(algorithm_shared Core Multimedia OpenGL)
