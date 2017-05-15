project(augmentation_test)

find_package(Qt5 REQUIRED Core Gui Widgets OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set( augmentation_test_SOURCES   main.cpp window.cpp )
set( augmentation_test_HEADERS   window.hpp )
set( augmentation_test_FORMS     window.ui )

qt5_wrap_ui(augmentation_test_UI_HEADERS ${augmentation_test_FORMS})
qt5_add_resources(augmentation_rcc
    ${CMAKE_SOURCE_DIR}/resources/3D_models/3D_models.qrc
    ${CMAKE_SOURCE_DIR}/resources/debug_samples/debug_samples.qrc
    ${CMAKE_SOURCE_DIR}/resources/GL_shaders/GL_shaders.qrc
)

add_executable( augmentation_test ${augmentation_test_SOURCES}
                ${augmentation_test_UI_HEADERS} ${augmentation_rcc})
target_link_libraries(augmentation_test augmentation ${QT_LIBRARIES} )

qt5_use_modules(augmentation_test Core Gui Widgets OpenGL)
