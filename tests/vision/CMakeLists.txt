find_package(Qt6 COMPONENTS Core Gui Quick OpenGL)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Quick OpenGL)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set( vision_test_SOURCES   main.cpp frame_data_lister.cpp)
set( vision_test_HEADERS   frame_data_lister.hpp)

qt_add_resources(vision_rcc
    vision_test.qrc
    ${CMAKE_SOURCE_DIR}/resources/3D_models/3D_models.qrc
    ${CMAKE_SOURCE_DIR}/resources/debug_samples/debug_samples.qrc
)

add_executable( vision_test ${vision_test_SOURCES} ${vision_rcc} ${vision_test_HEADERS})
target_link_libraries(vision_test frame_data vision Qt::Core Qt::Gui Qt::Quick Qt::OpenGL)
