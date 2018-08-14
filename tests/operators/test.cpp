#include "operators/operators.hpp"

#include <cmath>
#include <map>

#include "gtest/gtest.h"

TEST (centroid, operators) {
    operators t_operators;
    // clang-format off
    /* centroid at {1, 1} */
    //1  2
    //3  4
    const points_t shape_a = {
        { 1, { 0, 2 } }, { 2, { 2, 2 } },
        { 3, { 0, 0 } }, { 4, { 2, 0 } }
    };
    /* centroid at {0.5, 0.5} */
    //1  2
    //3  4
    const points_t shape_a_f = {
        { 1, { 0, 1 } }, { 0, { 1, 1 } },
        { 3, { 0, 0 } }, { 4, { 1, 0 } }
    };
    /* centroid at {2, 2} */
    //  1
    //2   3
    const points_t shape_b = {
              { 1, { 2, 4 } },
        { 2, { 1, 1 } }, { 3, { 3, 1 } }
    };
    /* empty shape */
    const points_t shape_empty = {
    };
    // clang-format on
    /* shape to limit */
    const unsigned int shape_limit_points = 40;
    points_t shape_limit                  = {};
    for (unsigned int i = 1; i <= shape_limit_points; i++) {
        shape_limit.insert ({ i, { 65535, 65535 } });
    }
    /* shape A */
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_a).x, 1);
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_a).y, 1);
    /* shape A float */
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_a_f).x, 0.5);
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_a_f).y, 0.5);
    /* shape B */
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_b).x, 2);
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_b).y, 2);
    /* shape empty */
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_empty).x, 0);
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_empty).y, 0);
    /* shape limit */
    ASSERT_FLOAT_EQ (shape_limit.size (), shape_limit_points);
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_limit).x, 65535);
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_limit).y, 65535);
    /* shape over limit */
    shape_limit.insert ({ shape_limit_points + 1, { 65535, 65535 } }); // over limit
    ASSERT_FLOAT_EQ (shape_limit.size (), shape_limit_points + 1);
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_limit).x, 0);
    ASSERT_FLOAT_EQ (t_operators.centroid (shape_limit).y, 0);
}

TEST (intersections, operators) {
    operators To;
    const point_t o0_0   = { 0, 0 };
    const point_t o5_5   = { 5, 5 };
    const point_t o10_10 = { 10, 10 };
    const point_t p0_0   = { 0, 0 };
    const point_t p0_10  = { 0, 10 };
    const point_t p10_0  = { 10, 0 };
    const point_t p10_10 = { 10, 10 };
    // origin 0, 0
    // simple line
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_0, o0_0).x, 10);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_0, o0_0).y, 10);
    // horizontal line (y != 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_10, o0_0).x, POINT_INF);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_10, o0_0).y, 10);
    // horizontal line (y == 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o0_0).x, POINT_INF);
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o0_0).y, 0);
    // vertical line (x != 0)
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o0_0).x, 10);
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o0_0).y, POINT_INF);
    // vertical line (x == 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o0_0).x, 0);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o0_0).y, POINT_INF);

    // origin 5, 5
    // simple line
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_0, o5_5).x, 0);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_0, o5_5).y, 0);
    // horizontal line (y != 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_10, o5_5).x, POINT_INF);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_10, o5_5).y, 5);
    // horizontal line (y == 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o5_5).x, POINT_INF);
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o5_5).y, -5);
    // vertical line (x != 0)
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o5_5).x, 5);
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o5_5).y, POINT_INF);
    // vertical line (x == 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o5_5).x, -5);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o5_5).y, POINT_INF);

    // origin 10, 10
    // simple line
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_0, o10_10).x, -10);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_0, o10_10).y, -10);
    // horizontal line (y != 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_10, o10_10).x, POINT_INF);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_10, o10_10).y, 0);
    // horizontal line (y == 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o10_10).x, POINT_INF);
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o10_10).y, -10);
    // vertical line (x != 0)
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o10_10).x, 0);
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o10_10).y, POINT_INF);
    // vertical line (x == 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o10_10).x, -10);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o10_10).y, POINT_INF);
}

TEST (distance, operators) {
    operators To;
    ASSERT_FLOAT_EQ (To.distance ({ 0, 0 }, { 0, 0 }), 0);
    ASSERT_FLOAT_EQ (To.distance ({ 0, 0 }, { 10, 0 }), 10);
    ASSERT_FLOAT_EQ (To.distance ({ 0, 0 }, { 10, 10 }), 10 * std::sqrt (2));
    ASSERT_FLOAT_EQ (To.distance ({ -10, 10 }, { 10, -10 }), 20 * std::sqrt (2));
    ASSERT_FLOAT_EQ (To.distance ({ -10, -10 }, { -5, -5 }), 5 * std::sqrt (2));
}

TEST (line_A_B, operators) {
    operators To;
    // dot (points same value)
    ASSERT_EQ (std::isnan (To.a ({ { 0, 0 }, { 0, 0 } })), true);
    ASSERT_EQ (std::isnan (To.b ({ { 0, 0 }, { 0, 0 } })), true);
    // INF (points inf)
    ASSERT_EQ (
    std::isnan (To.a ({ { POINT_INF, POINT_INF }, { POINT_INF, POINT_INF } })), true);
    ASSERT_EQ (
    std::isnan (To.b ({ { POINT_INF, POINT_INF }, { POINT_INF, POINT_INF } })), true);

    // horizontal line, origin
    ASSERT_FLOAT_EQ (To.a ({ { 0, 0 }, { 10, 0 } }), 0);
    ASSERT_FLOAT_EQ (To.b ({ { 0, 0 }, { 10, 0 } }), 0);
    // horizontal line, +5
    ASSERT_FLOAT_EQ (To.a ({ { 0, 5 }, { 10, 5 } }), 0);
    ASSERT_FLOAT_EQ (To.b ({ { 0, 5 }, { 10, 5 } }), 5);
    // horizontal line, -5
    ASSERT_FLOAT_EQ (To.a ({ { 0, -5 }, { 10, -5 } }), 0);
    ASSERT_FLOAT_EQ (To.b ({ { 0, -5 }, { 10, -5 } }), -5);

    // vertical line, origin
    ASSERT_FLOAT_EQ (To.a ({ { 0, 0 }, { 0, 10 } }), POINT_INF);
    ASSERT_FLOAT_EQ (To.b ({ { 0, 0 }, { 0, 10 } }), POINT_INF);
    // horizontal line, +5
    ASSERT_FLOAT_EQ (To.a ({ { 5, 0 }, { 5, 10 } }), POINT_INF);
    ASSERT_FLOAT_EQ (To.b ({ { 5, 0 }, { 5, 10 } }), POINT_INF);
    // horizontal line, -5
    ASSERT_FLOAT_EQ (To.a ({ { -5, 0 }, { -5, -10 } }), POINT_INF);
    ASSERT_FLOAT_EQ (To.b ({ { -5, 0 }, { -5, -10 } }), POINT_INF);

    // 45 degrees line, positive, origin
    ASSERT_FLOAT_EQ (To.a ({ { 0, 0 }, { 10, 10 } }), 1);
    ASSERT_FLOAT_EQ (To.b ({ { 0, 0 }, { 10, 10 } }), 0);
    // 45 degrees line, negative, origin
    ASSERT_FLOAT_EQ (To.a ({ { 0, 0 }, { 10, -10 } }), -1);
    ASSERT_FLOAT_EQ (To.b ({ { 0, 0 }, { 10, -10 } }), 0);
    // 45 degrees line, positive, +5
    ASSERT_FLOAT_EQ (To.a ({ { 0, 5 }, { 10, 15 } }), 1);
    ASSERT_FLOAT_EQ (To.b ({ { 0, 5 }, { 10, 15 } }), 5);
    // 45 degrees line, negative, -5
    ASSERT_FLOAT_EQ (To.a ({ { 0, -5 }, { 10, -15 } }), -1);
    ASSERT_FLOAT_EQ (To.b ({ { 0, -5 }, { 10, -15 } }), -5);
}

TEST (line_X, operators) {
    operators To;
    line_t h_0   = { { 0, 0 }, { 1, 0 } };  // ―
    line_t v_0   = { { 0, 0 }, { 0, 1 } };  // |
    line_t a_45  = { { 0, 0 }, { 1, 1 } };  // ⟋
    line_t a_135 = { { 0, 0 }, { -1, 1 } }; // ⟍

    // horizontal line
    ASSERT_FLOAT_EQ (To.x (10, h_0), POINT_INF);
    ASSERT_FLOAT_EQ (To.x (0, h_0), POINT_INF);
    ASSERT_FLOAT_EQ (To.x (-10, h_0), POINT_INF);

    // vertical line
    ASSERT_FLOAT_EQ (To.x (10, v_0), 0);
    ASSERT_FLOAT_EQ (To.x (0, v_0), 0);
    ASSERT_FLOAT_EQ (To.x (-10, v_0), 0);

    // 45 degrees line
    ASSERT_FLOAT_EQ (To.x (10, a_45), 10);
    ASSERT_FLOAT_EQ (To.x (0, a_45), 0);
    ASSERT_FLOAT_EQ (To.x (-10, a_45), -10);

    // 135 degrees line
    ASSERT_FLOAT_EQ (To.x (10, a_135), -10);
    ASSERT_FLOAT_EQ (To.x (0, a_135), 0);
    ASSERT_FLOAT_EQ (To.x (-10, a_135), 10);
}

TEST (line_Y, operators) {
    operators To;
    line_t h_0   = { { 0, 0 }, { 1, 0 } };  // ―
    line_t v_0   = { { 0, 0 }, { 0, 1 } };  // |
    line_t a_45  = { { 0, 0 }, { 1, 1 } };  // ⟋
    line_t a_135 = { { 0, 0 }, { -1, 1 } }; // ⟍

    // horizontal line
    ASSERT_FLOAT_EQ (To.y (10, h_0), 0);
    ASSERT_FLOAT_EQ (To.y (0, h_0), 0);
    ASSERT_FLOAT_EQ (To.y (-10, h_0), 0);

    // vertical line
    ASSERT_FLOAT_EQ (To.y (10, v_0), POINT_INF);
    ASSERT_FLOAT_EQ (To.y (0, v_0), POINT_INF);
    ASSERT_FLOAT_EQ (To.y (-10, v_0), POINT_INF);

    // 45 degrees line
    ASSERT_FLOAT_EQ (To.y (10, a_45), 10);
    ASSERT_FLOAT_EQ (To.y (0, a_45), 0);
    ASSERT_FLOAT_EQ (To.y (-10, a_45), -10);

    // 135 degrees line
    ASSERT_FLOAT_EQ (To.y (10, a_135), -10);
    ASSERT_FLOAT_EQ (To.y (0, a_135), 0);
    ASSERT_FLOAT_EQ (To.y (-10, a_135), 10);
}

TEST (line_intersection, operators) {
    operators To;
    // random selected numbers
    line_t h     = { { 8, 12 }, { 12, 12 } }; // ―
    line_t v     = { { 5, 7 }, { 5, 12 } };   // |
    line_t a_45  = { { 8, 4 }, { 11, 7 } };   // ⟋
    line_t a_135 = { { 3, 5 }, { 6, 2 } };    // ⟍
    // horizontal
    //  - horizontal (parallel)
    EXPECT_FLOAT_EQ (To.intersection (h, h).x, POINT_INF);
    EXPECT_FLOAT_EQ (To.intersection (h, h).y, POINT_INF);
    //  - vertical
    EXPECT_FLOAT_EQ (To.intersection (h, v).x, 5);
    EXPECT_FLOAT_EQ (To.intersection (h, v).y, 12);
    //  - 45 degrees
    EXPECT_FLOAT_EQ (To.intersection (h, a_45).x, 16);
    EXPECT_FLOAT_EQ (To.intersection (h, a_45).y, 12);
    //  - 135 degrees
    EXPECT_FLOAT_EQ (To.intersection (h, a_135).x, -4);
    EXPECT_FLOAT_EQ (To.intersection (h, a_135).y, 12);
    // vertical
    //  - vertical (parallel)
    EXPECT_FLOAT_EQ (To.intersection (v, v).x, POINT_INF);
    EXPECT_FLOAT_EQ (To.intersection (v, v).y, POINT_INF);
    //  - horizontal
    EXPECT_FLOAT_EQ (To.intersection (v, h).x, 5);
    EXPECT_FLOAT_EQ (To.intersection (v, h).y, 12);
    //  - 45 degrees
    EXPECT_FLOAT_EQ (To.intersection (v, a_45).x, 5);
    EXPECT_FLOAT_EQ (To.intersection (v, a_45).y, 1);
    //  - 135 degrees
    EXPECT_FLOAT_EQ (To.intersection (v, a_135).x, 5);
    EXPECT_FLOAT_EQ (To.intersection (v, a_135).y, 3);
}

TEST (is_in_front, operators) {
    operators To;
    line_t l_h_h = { { 0, 0 }, { 1, 0 } };
    line_t l_h_v = { { 0, 0 }, { 0, 1 } };
    line_t h_l_h = { { 1, 0 }, { 0, 0 } };
    line_t h_l_v = { { 0, 1 }, { 0, 0 } };
    // low to high
    // horizontal
    EXPECT_EQ (To.is_in_front (l_h_h, l_h_h.p1, { -2, 0 }), true);
    EXPECT_EQ (To.is_in_front (l_h_h, l_h_h.p1, { -1, 0 }), true);
    EXPECT_EQ (To.is_in_front (l_h_h, l_h_h.p1, { 0, 0 }), false);
    EXPECT_EQ (To.is_in_front (l_h_h, l_h_h.p1, { 1, 0 }), false);
    // vertical
    EXPECT_EQ (To.is_in_front (l_h_v, l_h_v.p1, { 0, -2 }), true);
    EXPECT_EQ (To.is_in_front (l_h_v, l_h_v.p1, { 0, -1 }), true);
    EXPECT_EQ (To.is_in_front (l_h_v, l_h_v.p1, { 0, 0 }), false);
    EXPECT_EQ (To.is_in_front (l_h_v, l_h_v.p1, { 0, 1 }), false);
    // high to low
    // horizontal
    EXPECT_EQ (To.is_in_front (h_l_h, h_l_h.p1, { 3, 0 }), true);
    EXPECT_EQ (To.is_in_front (h_l_h, h_l_h.p1, { 2, 0 }), true);
    EXPECT_EQ (To.is_in_front (h_l_h, h_l_h.p1, { 1, 0 }), false);
    EXPECT_EQ (To.is_in_front (h_l_h, h_l_h.p1, { 0, 0 }), false);
    // vertical
    EXPECT_EQ (To.is_in_front (h_l_v, h_l_v.p1, { 0, 3 }), true);
    EXPECT_EQ (To.is_in_front (h_l_v, h_l_v.p1, { 0, 2 }), true);
    EXPECT_EQ (To.is_in_front (h_l_v, h_l_v.p1, { 0, 1 }), false);
    EXPECT_EQ (To.is_in_front (h_l_v, h_l_v.p1, { 0, 0 }), false);
}

TEST (closest, operators) {
    operators To;
    std::vector<float> v = { 1, 2, 4, 5, 7, 8 };
    EXPECT_FLOAT_EQ (To.closest (v, -1.0), 1.0);
    EXPECT_FLOAT_EQ (To.closest (v, 0.0), 1.0);
    EXPECT_FLOAT_EQ (To.closest (v, 1.0), 1.0);
    EXPECT_FLOAT_EQ (To.closest (v, 1.4), 1.0);

    EXPECT_FLOAT_EQ (To.closest (v, 1.5), 2.0);
    EXPECT_FLOAT_EQ (To.closest (v, 2.0), 2.0);
    EXPECT_FLOAT_EQ (To.closest (v, 2.4), 2.0);
    EXPECT_FLOAT_EQ (To.closest (v, 2.5), 2.0);

    EXPECT_FLOAT_EQ (To.closest (v, 3.0), 4.0);
    EXPECT_FLOAT_EQ (To.closest (v, 3.4), 4.0);
    EXPECT_FLOAT_EQ (To.closest (v, 3.5), 4.0);
    EXPECT_FLOAT_EQ (To.closest (v, 4.4), 4.0);

    EXPECT_FLOAT_EQ (To.closest (v, 4.5), 5.0);
}

TEST (projected_angle_abs, operators) {
    operators To;

    EXPECT_FLOAT_EQ (To.projected_angle_abs (1, std::sqrt (2) / 2), 45);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (std::sqrt (2) / 2, 1), 45);

    EXPECT_FLOAT_EQ (To.projected_angle_abs (10, 0.000001), 90);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (0.000001, 10), 90);

    EXPECT_FLOAT_EQ (To.projected_angle_abs (10, 10), 0);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (0, 0), 0);

    EXPECT_FLOAT_EQ (To.projected_angle_abs (0, 1), 0);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (1, 0), 0);

    EXPECT_FLOAT_EQ (To.projected_angle_abs (POINT_INF, 1), 90);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (POINT_INF, 10), 90);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (POINT_INF, 100), 90);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (POINT_INF, 10000), 90);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (1, POINT_INF), 90);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (10, POINT_INF), 90);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (100, POINT_INF), 90);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (10000, POINT_INF), 90);

    EXPECT_FLOAT_EQ (To.projected_angle_abs (std::nanf ("1"), 1), 0);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (1, std::nanf ("1")), 0);
    EXPECT_FLOAT_EQ (To.projected_angle_abs (std::nanf ("1"), std::nanf ("1")), 0);
}

TEST (dot_product, operators) {
    operators To;
    // horizontal point
    const point_t hp_1  = { 1, 0 };
    const point_t hp_10 = { 10, 0 };
    // vertical point
    const point_t vp_1  = { 0, 1 };
    const point_t vp_10 = { 0, 1 };

    // edge cases
    // zero/same value
    EXPECT_FLOAT_EQ (To.dot_product ({ 0, 0 }, { 0, 0 }), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, hp_1), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_10, hp_10), 0);
    // NaN
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { std::nanf ("1"), std::nanf ("1") }), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { 0, std::nanf ("1") }), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { 1, std::nanf ("1") }), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { std::nanf ("1"), 0 }), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { std::nanf ("1"), 1 }), 0);
    EXPECT_FLOAT_EQ (To.dot_product ({ 0, std::nanf ("1") }, hp_1), 0);
    EXPECT_FLOAT_EQ (To.dot_product ({ 1, std::nanf ("1") }, hp_1), 0);
    EXPECT_FLOAT_EQ (To.dot_product ({ std::nanf ("1"), 0 }, hp_1), 0);
    EXPECT_FLOAT_EQ (To.dot_product ({ std::nanf ("1"), 1 }, hp_1), 0);
    // INF
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { POINT_INF, POINT_INF }), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { 0, POINT_INF }), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { 1, POINT_INF }), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { POINT_INF, 0 }), 0);
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { POINT_INF, 1 }), 0);
    EXPECT_FLOAT_EQ (To.dot_product ({ 0, POINT_INF }, hp_1), 0);
    EXPECT_FLOAT_EQ (To.dot_product ({ 1, POINT_INF }, hp_1), 0);
    EXPECT_FLOAT_EQ (To.dot_product ({ POINT_INF, 0 }, hp_1), 0);
    EXPECT_FLOAT_EQ (To.dot_product ({ POINT_INF, 1 }, hp_1), 0);

    // horizontal point
    /* 45 degrees */
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { 1, 1 }), 0.25 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ 1, 1 }, hp_1), -0.25 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (hp_1, { 1, 1 }), 45);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ 1, 1 }, hp_1), -45);

    EXPECT_FLOAT_EQ (To.dot_product (hp_10, { 1, 1 }), 0.25 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ 1, 1 }, hp_10), -0.25 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (hp_10, { 1, 1 }), 45);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ 1, 1 }, hp_10), -45);

    /* 90 degrees */
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { 0, 1 }), 0.5 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ 0, 1 }, hp_1), -0.5 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (hp_1, { 0, 1 }), 90);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ 0, 1 }, hp_1), -90);

    EXPECT_FLOAT_EQ (To.dot_product (hp_10, { 0, 1 }), 0.5 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ 0, 1 }, hp_10), -0.5 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (hp_10, { 0, 1 }), 90);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ 0, 1 }, hp_10), -90);

    /* 135 degrees */
    EXPECT_FLOAT_EQ (To.dot_product (hp_1, { -1, -1 }), -0.75 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ -1, -1 }, hp_1), 0.75 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (hp_1, { -1, -1 }), -135);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ -1, -1 }, hp_1), 135);

    EXPECT_FLOAT_EQ (To.dot_product (hp_10, { -1, -1 }), -0.75 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ -1, -1 }, hp_10), 0.75 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (hp_10, { -1, -1 }), -135);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ -1, -1 }, hp_10), 135);

    // vertical point
    /* 45 degrees */
    EXPECT_FLOAT_EQ (To.dot_product (vp_1, { 1, 1 }), -0.25 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ 1, 1 }, vp_1), 0.25 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (vp_1, { 1, 1 }), -45);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ 1, 1 }, vp_1), 45);

    EXPECT_FLOAT_EQ (To.dot_product (vp_10, { 1, 1 }), -0.25 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ 1, 1 }, vp_10), 0.25 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (vp_10, { 1, 1 }), -45);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ 1, 1 }, vp_10), 45);

    /* 90 degrees */
    EXPECT_FLOAT_EQ (To.dot_product (vp_1, { 1, 0 }), -0.5 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ 1, 0 }, vp_1), 0.5 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (vp_1, { 1, 0 }), -90);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ 1, 0 }, vp_1), 90);

    EXPECT_FLOAT_EQ (To.dot_product (vp_10, { 1, 0 }), -0.5 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ 1, 0 }, vp_10), 0.5 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (vp_10, { 1, 0 }), -90);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ 1, 0 }, vp_10), 90);

    /* 135 degrees */
    EXPECT_FLOAT_EQ (To.dot_product (vp_1, { -1, -1 }), 0.75 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ -1, -1 }, vp_1), -0.75 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (vp_1, { -1, -1 }), 135);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ -1, -1 }, vp_1), -135);

    EXPECT_FLOAT_EQ (To.dot_product (vp_10, { -1, -1 }), 0.75 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product ({ -1, -1 }, vp_10), -0.75 * M_PI);
    EXPECT_FLOAT_EQ (To.dot_product_degrees (vp_10, { -1, -1 }), 135);
    EXPECT_FLOAT_EQ (To.dot_product_degrees ({ -1, -1 }, vp_10), -135);
}

TEST (translate, operators) {
    operators To;
    // clang-format off
    const points_t base = {
        { 1, { 0, 0 } }, { 2, { 42, 1337 } }
    };
    // clang-format on
    points_t ref;

    // NaN
    ref = base;
    To.translate (ref, { std::nanf ("1"), std::nanf ("1") });
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0);
    EXPECT_FLOAT_EQ (ref.at (2).x, 42);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1337);
    ref = base;
    To.translate (ref, { 0, std::nanf ("1") });
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0);
    EXPECT_FLOAT_EQ (ref.at (2).x, 42);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1337);
    ref = base;
    To.translate (ref, { std::nanf ("1"), 0 });
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0);
    EXPECT_FLOAT_EQ (ref.at (2).x, 42);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1337);

    // INF
    ref = base;
    To.translate (ref, { POINT_INF, POINT_INF });
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0);
    EXPECT_FLOAT_EQ (ref.at (2).x, 42);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1337);
    ref = base;
    To.translate (ref, { 0, POINT_INF });
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0);
    EXPECT_FLOAT_EQ (ref.at (2).x, 42);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1337);
    ref = base;
    To.translate (ref, { POINT_INF, 0 });
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0);
    EXPECT_FLOAT_EQ (ref.at (2).x, 42);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1337);

    /* T (0,0) */
    ref = base;
    To.translate (ref, { 0, 0 });
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0);
    EXPECT_FLOAT_EQ (ref.at (2).x, 42);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1337);
    /* T (1,0) */
    ref = base;
    To.translate (ref, { 1, 0 });
    EXPECT_FLOAT_EQ (ref.at (1).x, 1);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0);
    EXPECT_FLOAT_EQ (ref.at (2).x, 43);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1337);
    /* T (0,1) */
    ref = base;
    To.translate (ref, { 0, 1 });
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 1);
    EXPECT_FLOAT_EQ (ref.at (2).x, 42);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1338);
    /* T (1,1) */
    ref = base;
    To.translate (ref, { 1, 1 });
    EXPECT_FLOAT_EQ (ref.at (1).x, 1);
    EXPECT_FLOAT_EQ (ref.at (1).y, 1);
    EXPECT_FLOAT_EQ (ref.at (2).x, 43);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1338);
    /* T (-1,0) */
    ref = base;
    To.translate (ref, { -1, 0 });
    EXPECT_FLOAT_EQ (ref.at (1).x, -1);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0);
    EXPECT_FLOAT_EQ (ref.at (2).x, 41);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1337);
    /* T (0,-1) */
    ref = base;
    To.translate (ref, { 0, -1 });
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, -1);
    EXPECT_FLOAT_EQ (ref.at (2).x, 42);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1336);
    /* T (-1,-1) */
    ref = base;
    To.translate (ref, { -1, -1 });
    EXPECT_FLOAT_EQ (ref.at (1).x, -1);
    EXPECT_FLOAT_EQ (ref.at (1).y, -1);
    EXPECT_FLOAT_EQ (ref.at (2).x, 41);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1336);
    /* T (123,456) */
    ref = base;
    To.translate (ref, { 123, 456 });
    EXPECT_FLOAT_EQ (ref.at (1).x, 123);
    EXPECT_FLOAT_EQ (ref.at (1).y, 456);
    EXPECT_FLOAT_EQ (ref.at (2).x, 165);
    EXPECT_FLOAT_EQ (ref.at (2).y, 1793);
    /* T (-789,-963) */
    ref = base;
    To.translate (ref, { -789, -963 });
    EXPECT_FLOAT_EQ (ref.at (1).x, -789);
    EXPECT_FLOAT_EQ (ref.at (1).y, -963);
    EXPECT_FLOAT_EQ (ref.at (2).x, -747);
    EXPECT_FLOAT_EQ (ref.at (2).y, 374);
}

TEST (scale, operators) {
    operators To;
    // clang-format off
    const points_t base = {
        { 1, { 0, 1 } }, { 2, { 1, 0 } }
    };
    // clang-format on
    points_t ref;

    // NaN
    ref = base;
    To.scale (ref, std::nanf ("1"));
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 1);
    EXPECT_FLOAT_EQ (ref.at (2).x, 1);
    EXPECT_FLOAT_EQ (ref.at (2).y, 0);

    // INF
    ref = base;
    To.scale (ref, POINT_INF);
    EXPECT_FLOAT_EQ (ref.at (1).x, 0);
    EXPECT_FLOAT_EQ (ref.at (1).y, 1);
    EXPECT_FLOAT_EQ (ref.at (2).x, 1);
    EXPECT_FLOAT_EQ (ref.at (2).y, 0);

    /* S:0 centroid */
    ref = base;
    To.scale (ref, 0);
    EXPECT_FLOAT_EQ (ref.at (1).x, 0.5);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0.5);
    EXPECT_FLOAT_EQ (ref.at (2).x, 0.5);
    EXPECT_FLOAT_EQ (ref.at (2).y, 0.5);
    /* S:2 */
    ref = base;
    To.scale (ref, 2);
    EXPECT_FLOAT_EQ (ref.at (1).x, -0.5);
    EXPECT_FLOAT_EQ (ref.at (1).y, 1.5);
    EXPECT_FLOAT_EQ (ref.at (2).x, 1.5);
    EXPECT_FLOAT_EQ (ref.at (2).y, -0.5);
    /* S:0.5 */
    ref = base;
    To.scale (ref, 0.5);
    EXPECT_FLOAT_EQ (ref.at (1).x, 0.25);
    EXPECT_FLOAT_EQ (ref.at (1).y, 0.75);
    EXPECT_FLOAT_EQ (ref.at (2).x, 0.75);
    EXPECT_FLOAT_EQ (ref.at (2).y, 0.25);
}
