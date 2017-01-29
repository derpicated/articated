project(movement3d_filter_test)
add_executable(movement3d_filter_test test.cpp)
target_link_libraries(
    movement3d_filter_test
        movement3d_filter
        movement3d
        gtest
        gtest_main
)
add_test(
    NAME movement3d_filter_test
    COMMAND movement3d_filter_test
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)
