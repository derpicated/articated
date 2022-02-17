find_package(Qt6 COMPONENTS Core Gui Quick OpenGL)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Quick OpenGL)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set( augmentation_test_SOURCES   main.cpp mock_algorithm.cpp)
set( augmentation_test_HEADERS   mock_algorithm.hpp)

qt_add_resources(augmentation_rcc
    augmentation_test.qrc
    ${CMAKE_SOURCE_DIR}/resources/3D_models/3D_models.qrc
    ${CMAKE_SOURCE_DIR}/resources/debug_samples/debug_samples.qrc
)

add_executable( augmentation_test ${augmentation_test_SOURCES} ${augmentation_rcc}
                ${augmentation_test_UI_HEADERS})
target_link_libraries(augmentation_test frame_data augmentation Qt::Core Qt::Gui Qt::Quick Qt::OpenGL)
