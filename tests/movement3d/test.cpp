#include "shared/movement3d/movement3d.hpp"
#include <gtest/gtest.h>

TEST (movement3d_translation_delta_to_absolute_nofail, Movement3D) {
    Movement3D movement3d_test;
    //          x         no movement (delta is 0)
    // -1       0        1
    // | ------ | ------ |
    ASSERT_FLOAT_EQ (movement3d_test.translation_delta_to_absolute (0, 100, -1.0, 1.0), 0);

    //          x -->10   10 points to right
    // -1       0        1
    // | ------ | ------ |
    ASSERT_FLOAT_EQ (
    movement3d_test.translation_delta_to_absolute (10, 100, -1.0, 1.0), 0.2);

    //    10<-- x         10 points to left
    // -1       0        1
    // | ------ | ------ |
    ASSERT_FLOAT_EQ (
    movement3d_test.translation_delta_to_absolute (-10, 100, -1.0, 1.0), -0.2);

    //          x -->50   50 points to right
    // -1       0        1
    // | ------ | ------ |
    ASSERT_FLOAT_EQ (
    movement3d_test.translation_delta_to_absolute (50, 100, -1.0, 1.0), 1.0);


    //    50<-- x         50 points to left
    // -1       0        1
    // | ------ | ------ |
    ASSERT_FLOAT_EQ (
    movement3d_test.translation_delta_to_absolute (-50, 100, -1.0, 1.0), -1.0);
}

TEST (movement3d_operator_plus_no_fail, Movement3D) {
    Movement3D x1, x2;

    x1.scale (1.1);
    x1.translation ({ 1.2, 1.3 });
    x1.yaw (1.4);
    x1.pitch (1.5);
    x1.roll (1.6);

    x2.scale (2.1);
    x2.translation ({ 2.2, 2.3 });
    x2.yaw (2.4);
    x2.pitch (2.5);
    x2.roll (2.6);

    x1 += x2;

    ASSERT_FLOAT_EQ (x1.scale (), 3.2);

    ASSERT_FLOAT_EQ (x1.translation ().x, 3.4);
    ASSERT_FLOAT_EQ (x1.translation ().y, 3.6);

    ASSERT_FLOAT_EQ (x1.yaw (), 3.8);
    ASSERT_FLOAT_EQ (x1.pitch (), 4.0);
    ASSERT_FLOAT_EQ (x1.roll (), 4.2);
}

TEST (movement3d_operator_divide_equal_integer_no_fail, Movement3D) {
    Movement3D x1;

    x1.scale (1.1);
    x1.translation ({ 1.2, 1.3 });
    x1.yaw (1.4);
    x1.pitch (1.5);
    x1.roll (1.6);

    x1 /= 2;
    ASSERT_FLOAT_EQ (x1.scale (), 0.55);

    ASSERT_FLOAT_EQ (x1.translation ().x, 0.6);
    ASSERT_FLOAT_EQ (x1.translation ().y, 0.65);

    ASSERT_FLOAT_EQ (x1.yaw (), 0.7);
    ASSERT_FLOAT_EQ (x1.pitch (), 0.75);
    ASSERT_FLOAT_EQ (x1.roll (), 0.8);
}
