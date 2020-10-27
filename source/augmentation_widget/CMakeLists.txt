find_package(Qt5 REQUIRED Core Gui Quick OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( augmentation_SOURCES augmentation_view.cpp augmentation_renderer.cpp model_loader.cpp)
set( augmentation_HEADERS  augmentation_view.hpp augmentation_renderer.hpp model_loader.hpp)
qt5_wrap_cpp(augmentation_MOC ${augmentation_HEADERS})
qt5_add_resources(augmentation_RRC
    shaders/GL_shaders.qrc
    ${CMAKE_SOURCE_DIR}/resources/3D_models/3D_models.qrc
)

add_library( augmentation STATIC ${augmentation_SOURCES} ${augmentation_RRC} ${augmentation_MOC} )
if(ANDROID)
    target_link_libraries( augmentation ${QT_LIBRARIES} frame_data movement3d GLESv3 )
else()
    target_link_libraries( augmentation ${QT_LIBRARIES} frame_data movement3d )
endif()
target_include_directories ( augmentation PUBLIC ${INCLUDE_DIR} )

qt5_use_modules( augmentation Core Gui OpenGL )
