add_executable(movement3d_test test.cpp)
target_link_libraries(
    movement3d_test
        movement3d
        gtest
        gtest_main
)

add_test(
    NAME movement3d_test
    COMMAND movement3d_test
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
