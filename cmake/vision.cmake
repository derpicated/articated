find_package(Qt5 REQUIRED Core Gui Multimedia OpenGL)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(VISION_INC
    ${INCLUDE_DIR}/vision.hpp
    ${INCLUDE_DIR}/acquisition.hpp
    ${INCLUDE_DIR}/vision_algorithm.hpp
    ${INCLUDE_DIR}/algorithm_original.hpp
)
set(VISION_SRC
    ${SRC_DIR}/vision.cpp
    ${SRC_DIR}/acquisition.cpp
    ${SRC_DIR}/vision_algorithm.cpp
    ${SRC_DIR}/algorithm_original.cpp
)
qt5_wrap_cpp(vision_MOC ${VISION_INC})

add_library( vision STATIC ${VISION_SRC} ${VISION_HPP} ${vision_MOC} )
target_link_libraries( vision ${QT_LIBRARIES} operators movement3d movement3d_filter)
target_include_directories ( vision PUBLIC ${INCLUDE_DIR} )

target_compile_options(vision PRIVATE -std=c++11 -Wall -Wextra)

qt5_use_modules( vision Core Gui Multimedia OpenGL)
