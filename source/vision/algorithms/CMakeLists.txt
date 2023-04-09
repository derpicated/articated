# vision
find_package(Qt6 REQUIRED COMPONENTS Core Multimedia OpenGL)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_add_resources(ALGORITHM_RESOURCES gpu/shaders/vision_gpu_shaders.qrc)

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

# TODO(articated #44) Remove Qt6::MultemediaPrivate dependency once a suitable replacement
#  has been found for the QAbstractVideoBuffer API.
target_link_libraries(algorithms
        frame_data movement3d movement3d_filter
        Qt::Core Qt::Multimedia Qt6::MultimediaPrivate Qt::OpenGL
        $<$<BOOL:${ANDROID}>:GLESv3>
        )
