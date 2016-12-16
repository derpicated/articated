add_compile_options(-std=c++11) # CMake 2.8.12 or newer
#set(CMAKE_CXX_STANDARD 11) # CMake 3.1 and newer
add_compile_options(-Wall -Wextra)

add_subdirectory(classification)
