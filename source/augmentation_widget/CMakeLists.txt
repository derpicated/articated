find_package(Qt5 REQUIRED Core Gui Widgets OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( augmentation_SOURCES augmentation_widget.cpp model_loader.cpp)
set( augmentation_HEADERS augmentation_widget.hpp model_loader.hpp)
qt5_wrap_cpp(augmentation_MOC ${augmentation_HEADERS})
qt5_add_resources(augmentation_RRC shaders/GL_shaders.qrc)

add_library( augmentation STATIC ${augmentation_SOURCES} ${augmentation_RRC} ${augmentation_MOC} )
if(ANDROID)
    target_link_libraries( augmentation operators ${QT_LIBRARIES} GLESv3)
else()
    target_link_libraries( augmentation operators ${QT_LIBRARIES} )
endif()
target_include_directories ( augmentation PUBLIC ${INCLUDE_DIR} )

qt5_use_modules( augmentation Core Gui Widgets OpenGL )

target_compile_options(augmentation PRIVATE -std=c++11 -Wall -Wextra)
