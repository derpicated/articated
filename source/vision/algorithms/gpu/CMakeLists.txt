find_package(Qt5 REQUIRED Core Gui Multimedia OpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( algorithm_gpu_SOURCES algorithm_gpu.cpp)
set( algorithm_gpu_HEADERS algorithm_gpu.hpp)
qt5_wrap_cpp(algorithm_gpu_MOC ${algorithm_gpu_HEADERS})
qt5_add_resources(algorithm_gpu_RRC shaders/vision_gpu_shaders.qrc)

add_library( algorithm_gpu STATIC ${algorithm_gpu_SOURCES} ${algorithm_gpu_RRC} ${algorithm_gpu_MOC} )
if(ANDROID)
    target_link_libraries( algorithm_gpu algorithm_shared ${QT_LIBRARIES} GLESv3)
else()
    target_link_libraries( algorithm_gpu algorithm_shared ${QT_LIBRARIES} )
endif()
target_include_directories ( algorithm_gpu PUBLIC ${INCLUDE_DIR} )

qt5_use_modules( algorithm_gpu Core Gui Multimedia OpenGL )

target_compile_options(algorithm_gpu PRIVATE -std=c++11 -Wall -Wextra)
