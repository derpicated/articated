find_package(Qt6 COMPONENTS Core Gui Multimedia OpenGL)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Multimedia OpenGL)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(algorithms)

set(VISION_INC vision.hpp acquisition.hpp)
set(VISION_SRC vision.cpp acquisition.cpp)
qt_wrap_cpp(vision_MOC ${VISION_INC})

add_library( vision STATIC ${VISION_SRC} ${VISION_HPP} ${vision_MOC} )
target_link_libraries( vision Qt::Core Qt::Gui Qt::Multimedia Qt::OpenGL frame_data algorithms movement3d movement3d_filter)
target_include_directories ( vision PUBLIC ${INCLUDE_DIR} )
