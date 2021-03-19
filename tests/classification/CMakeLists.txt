add_executable(classification_demo demo.cpp)
target_link_libraries(
    classification_demo
    algorithms
)

add_executable(
    classification_test
    test.cpp
)
target_link_libraries(
    classification_test
    algorithms
    gtest
    gtest_main
)

add_test(
    NAME classification_test
    COMMAND classification_test
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
