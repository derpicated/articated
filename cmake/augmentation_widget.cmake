find_package(Qt5 REQUIRED Core Gui Widgets OpenGL)

if(ANDROID)
    add_definitions(-DOPENGL_ES)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( augmentation_SOURCES ${SRC_DIR}/augmentation_widget.cpp ${SRC_DIR}/model_loader.cpp)
set( augmentation_HEADERS ${INCLUDE_DIR}/augmentation_widget.hpp ${INCLUDE_DIR}/model_loader.hpp)
qt5_wrap_cpp(augmentation_MOC ${augmentation_HEADERS})

add_library( augmentation STATIC ${augmentation_SOURCES}  ${augmentation_MOC} )
if(ANDROID)
    target_link_libraries( augmentation ${QT_LIBRARIES} GLESv1_CM )
else()
    target_link_libraries( augmentation ${QT_LIBRARIES} GL)
endif()
target_include_directories ( augmentation PUBLIC ${INCLUDE_DIR} )

qt5_use_modules( augmentation Core Gui Widgets OpenGL )

target_compile_options(augmentation PRIVATE -std=c++11 -Wall -Wextra)
