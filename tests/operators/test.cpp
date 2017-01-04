#include "operators.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <map>

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
    shape_limit.insert (
    { shape_limit_points + 1, { 65535, 65535 } }); // over limit
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
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o0_0).x, 0);
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o0_0).y, 0);
    // vertical line (x != 0)
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o0_0).x, 10);
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o0_0).y, POINT_INF);
    // vertical line (x == 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o0_0).x, 0);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o0_0).y, 0);

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
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_10, o10_10).x, 0);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p10_10, o10_10).y, 0);
    // horizontal line (y == 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o10_10).x, POINT_INF);
    ASSERT_FLOAT_EQ (To.intersections (p0_0, p10_0, o10_10).y, -10);
    // vertical line (x != 0)
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o10_10).x, 0);
    ASSERT_FLOAT_EQ (To.intersections (p10_10, p10_0, o10_10).y, 0);
    // vertical line (x == 0)
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o10_10).x, -10);
    ASSERT_FLOAT_EQ (To.intersections (p0_10, p0_0, o10_10).y, POINT_INF);
}

TEST (distance, operators) {
    operators To;
    ASSERT_FLOAT_EQ (To.distance ({ 0, 0 }, { 10, 0 }), 10);
    ASSERT_FLOAT_EQ (To.distance ({ 0, 0 }, { 10, 10 }), 10 * std::sqrt (2));
    ASSERT_FLOAT_EQ (To.distance ({ -10, 10 }, { 10, -10 }), 20 * std::sqrt (2));
    ASSERT_FLOAT_EQ (To.distance ({ -10, -10 }, { -5, -5 }), 5 * std::sqrt (2));
}

TEST (line_A_B, operators) {
    operators To;
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
