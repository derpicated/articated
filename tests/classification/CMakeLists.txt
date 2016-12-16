project(classification_demo)

add_executable(classification_demo demo.cpp)
target_link_libraries(
    classification_demo
        operators
)

# project(classification_test)
# add_executable(classification_test test.cpp)
# target_link_libraries(
#     classification_test
#         vision_methods
#         movement3d
#         ${OpenCV_LIBS}
#         gtest
#         gtest_main
# )
# add_test(NAME classification_test
#          COMMAND classification_test
#          WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
