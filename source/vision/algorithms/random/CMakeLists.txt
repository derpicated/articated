find_package(Qt5 REQUIRED Core Gui Multimedia OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( algorithm_original_SOURCES algorithm_random.cpp)
set( algorithm_original_HEADERS algorithm_random.hpp)
qt5_wrap_cpp(algorithm_original_MOC ${algorithm_original_HEADERS})

add_library( algorithm_random STATIC ${algorithm_original_SOURCES} ${algorithm_original_MOC} )
if(ANDROID)
    target_link_libraries( algorithm_random ${QT_LIBRARIES} framedata GLESv3 )
else()
    target_link_libraries( algorithm_random ${QT_LIBRARIES} framedata )
endif()
target_include_directories ( algorithm_random PUBLIC ${INCLUDE_DIR} )

qt5_use_modules( algorithm_random Core Gui Multimedia OpenGL )
