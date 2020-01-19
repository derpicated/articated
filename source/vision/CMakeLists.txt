find_package(Qt5 REQUIRED Core Gui Multimedia OpenGL)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(algorithms)

set(VISION_INC vision.hpp acquisition.hpp)
set(VISION_SRC vision.cpp acquisition.cpp)
qt5_wrap_cpp(vision_MOC ${VISION_INC})

add_library( vision STATIC ${VISION_SRC} ${VISION_HPP} ${vision_MOC} )
target_link_libraries( vision ${QT_LIBRARIES} framedata algorithms augmentation movement3d movement3d_filter)
target_include_directories ( vision PUBLIC ${INCLUDE_DIR} )

qt5_use_modules(vision Core Gui Multimedia OpenGL)
