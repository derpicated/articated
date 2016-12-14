set(OPERATORS_INC
    ${INCLUDE_DIR}/operators.hpp
)
set(OPERATORS_SRC
    ${SRC_DIR}/operators.cpp
)

add_library( operators SHARED ${OPERATORS_SRC} ${OPERATORS_HPP} )
target_link_libraries( operators
)
target_include_directories ( operators PUBLIC ${INCLUDE_DIR} )

target_compile_options(operators PRIVATE -std=c++11 -Wall -Wextra)
