find_package(Qt5 REQUIRED Core Gui Multimedia OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( algorithm_brute_SOURCES algorithm_brute.cpp)
set( algorithm_brute_HEADERS algorithm_brute.hpp)
qt5_wrap_cpp(algorithm_brute_MOC ${algorithm_brute_HEADERS})
qt5_add_resources(algorithm_brute_RRC shaders/vision_brute_shaders.qrc)

add_library( algorithm_brute STATIC ${algorithm_brute_SOURCES} ${algorithm_brute_RRC} ${algorithm_brute_MOC} )
if(ANDROID)
    target_link_libraries( algorithm_brute algorithm_shared ${QT_LIBRARIES} GLESv3 frame_data )
else()
    target_link_libraries( algorithm_brute algorithm_shared ${QT_LIBRARIES} frame_data )
endif()
target_include_directories ( algorithm_brute PUBLIC ${INCLUDE_DIR} )

qt5_use_modules( algorithm_brute Core Gui Multimedia OpenGL )
