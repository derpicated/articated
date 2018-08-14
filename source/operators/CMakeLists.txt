set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(OPERATORS_INC operators.hpp)
set(OPERATORS_SRC operators.cpp)

add_library(operators STATIC ${OPERATORS_SRC} ${OPERATORS_HPP})
target_link_libraries(operators movement3d movement3d_filter)
target_compile_options(operators PRIVATE -std=c++11 -Wall -Wextra)
