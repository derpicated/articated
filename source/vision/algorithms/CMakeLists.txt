# vision
find_package(Qt5 REQUIRED Core Multimedia OpenGL)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_add_resources(ALGORITHM_RESOURCES gpu/shaders/vision_gpu_shaders.qrc)

add_library(
    algorithms STATIC
    utils/operators.cpp
    utils/classification.cpp
    utils/frame_helper.cpp
    gpu/algorithm_gpu.cpp
    random/algorithm_random.cpp
    original/algorithm_original.cpp
    ${ALGORITHM_RESOURCES}
)

target_link_libraries(algorithms frame_data movement3d movement3d_filter ${QT_LIBRARIES} $<$<BOOL:${ANDROID}>:GLESv3>)
target_include_directories(algorithms PUBLIC ${INCLUDE_DIR})
qt5_use_modules(algorithms Core Multimedia OpenGL)
