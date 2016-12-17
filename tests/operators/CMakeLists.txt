project(operators_demo)

add_executable(operators_demo demo.cpp)
target_link_libraries(
    operators_demo
        operators
)

project(operators_test)
add_executable(
    operators_test
        test.cpp
)
target_link_libraries(
    operators_test
        operators
        gtest
        gtest_main
)

add_test(
    NAME operators_test
    COMMAND operators_test
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
