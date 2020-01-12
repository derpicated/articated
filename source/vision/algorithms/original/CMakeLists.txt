find_package(Qt5 REQUIRED Core Gui Multimedia OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( algorithm_original_SOURCES algorithm_original.cpp)
set( algorithm_original_HEADERS algorithm_original.hpp)
qt5_wrap_cpp(algorithm_original_MOC ${algorithm_original_HEADERS})

add_library( algorithm_original STATIC ${algorithm_original_SOURCES} ${algorithm_original_MOC} )
if(ANDROID)
    target_link_libraries( algorithm_original algorithm_shared ${QT_LIBRARIES} GLESv3 framedata )
else()
    target_link_libraries( algorithm_original algorithm_shared ${QT_LIBRARIES} framedata )
endif()
target_include_directories ( algorithm_original PUBLIC ${INCLUDE_DIR} )

qt5_use_modules( algorithm_original Core Gui Multimedia OpenGL )
