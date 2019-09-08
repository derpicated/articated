#include "movement3d/movement3d.hpp"
#include "movement3d/movement3d_filter.hpp"
#include <gtest/gtest.h>

TEST (movement3d_filter_average_two_nofail, movement3d_filter) {
    Movement3DFilter filter (2);
    Movement3D x1, x2, av;

    x1.scale (0.0);
    x2.scale (1.1);

    x1.translation ({ 0.1, 0.2 });
    x2.translation ({ 1.1, 1.2 });

    x1.yaw (0.3);
    x2.yaw (1.3);

    x1.pitch (0.4);
    x2.pitch (1.4);

    x1.roll (0.5);
    x2.roll (1.5);

    filter.average (x1);
    filter.average (x2);
    av = filter.average ();

    ASSERT_FLOAT_EQ (av.scale (), 0.55);
    ASSERT_FLOAT_EQ (av.translation ().x, 0.6);
    ASSERT_FLOAT_EQ (av.translation ().y, 0.7);
    ASSERT_FLOAT_EQ (av.yaw (), 0.8);
    ASSERT_FLOAT_EQ (av.pitch (), 0.9);
    ASSERT_FLOAT_EQ (av.roll (), 1.0);
}

TEST (movement3d_filter_average_four_nofail, movement3d_filter) {
    Movement3DFilter filter (4);
    Movement3D x1, x2, x3, x4, av;

    x1.scale (0.0);
    x2.scale (1.0);
    x3.scale (2.0);
    x4.scale (3.0);

    x1.translation ({ 0.1, 0.2 });
    x2.translation ({ 1.1, 1.2 });
    x3.translation ({ 2.1, 2.2 });
    x4.translation ({ 3.1, 3.2 });

    x1.yaw (0.3);
    x2.yaw (1.3);
    x3.yaw (2.3);
    x4.yaw (3.3);

    x1.pitch (0.4);
    x2.pitch (1.4);
    x3.pitch (2.4);
    x4.pitch (3.4);

    x1.roll (0.5);
    x2.roll (1.5);
    x3.roll (2.5);
    x4.roll (3.5);

    filter.average (x1);
    filter.average (x2);
    filter.average (x3);
    filter.average (x4);
    av = filter.average ();

    ASSERT_FLOAT_EQ (av.scale (), 1.5);
    ASSERT_FLOAT_EQ (av.translation ().x, 1.6);
    ASSERT_FLOAT_EQ (av.translation ().y, 1.7);
    ASSERT_FLOAT_EQ (av.yaw (), 1.8);
    ASSERT_FLOAT_EQ (av.pitch (), 1.9);
    ASSERT_FLOAT_EQ (av.roll (), 2);
}
