find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick OpenGL)

set(CMAKE_AUTOMOC ON)

qt_add_resources(augmentation_rcc
    augmentation_test.qrc
    ${CMAKE_SOURCE_DIR}/resources/3D_models/3D_models.qrc
    ${CMAKE_SOURCE_DIR}/resources/debug_samples/debug_samples.qrc
)

add_executable(augmentation_test main.cpp mock_algorithm.cpp)
target_link_libraries(augmentation_test frame_data augmentation Qt::Core Qt::Gui Qt::Quick Qt::OpenGL)
