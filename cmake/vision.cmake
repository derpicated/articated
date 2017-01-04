find_package(Qt5 REQUIRED Core Gui Multimedia)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(VISION_INC
    ${INCLUDE_DIR}/vision.hpp
)
set(VISION_SRC
    ${SRC_DIR}/vision.cpp
)
qt5_wrap_cpp(vision_MOC ${vision_HEADERS})

add_library( vision SHARED ${VISION_SRC} ${VISION_HPP} ${vision_MOC} )
target_link_libraries( vision ${QT_LIBRARIES})
target_include_directories ( vision PUBLIC ${INCLUDE_DIR} )

target_compile_options(vision PRIVATE -std=c++11 -Wall -Wextra)

qt5_use_modules( vision Core Gui Multimedia )
