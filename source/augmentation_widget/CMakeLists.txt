find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_add_resources(
    augmentation_RESOURCES
    shaders/GL_shaders.qrc
    ${CMAKE_SOURCE_DIR}/resources/3D_models/3D_models.qrc
)

add_library(
    augmentation
    STATIC
    augmentation_view.cpp
    augmentation_renderer.cpp
    model_loader.cpp
    ${augmentation_RESOURCES}
)

target_link_libraries(augmentation frame_data movement3d Qt::Core Qt::Gui Qt::Quick Qt::OpenGL $<$<BOOL:${ANDROID}>:GLESv3>)
target_include_directories(augmentation PUBLIC ${INCLUDE_DIR})
