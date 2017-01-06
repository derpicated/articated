#include "operators.hpp"
#include <gtest/gtest.h>

TEST (translation, classification) {
    operators t_operators;
    // clang-format off
    /* reference */
    points_t reference = {
        { 1, { 100, 200 } }, { 2, { 200, 200 } },
        { 3, { 100, 100 } }, { 4, { 200, 100 } }
    };
    /* translate x: -2, Y: +3 */
    points_t t_m2_p3 = {
        { 1, { 98, 203 } }, { 2, { 198, 203 } },
        { 3, { 98, 103 } }, { 4, { 198, 103 } }
    };
    /* translate x: 5, Y: -42 */
    points_t t_p5_m42 = {
        { 1, { 105, 158 } }, { 2, { 205, 158 } },
        { 3, { 105, 58 } }, { 4, { 205, 58 } }
    };
    /* translate x: 0, Y: 0 */
    points_t t_p0_m0 = {
        { 1, { 100, 200 } }, { 2, { 200, 200 } },
        { 3, { 100, 100 } }, { 4, { 200, 100 } }
    };
    /* translate not enough points, so T=0,0 */
    points_t t_too_few_points = {
        { 1, { 100, 200 } }, { 2, { 200, 200 } }
    };
    // clang-format on
    translation_t translation = { 0, 0 };

    /* translate x: -2, Y: +3 */
    translation = t_operators.classify_translation (reference, t_m2_p3);
    ASSERT_EQ (translation.x, -2);
    ASSERT_EQ (translation.y, +3);

    /* translate x: 5, Y: -42 */
    translation = t_operators.classify_translation (reference, t_p5_m42);
    ASSERT_EQ (translation.x, +5);
    ASSERT_EQ (translation.y, -42);

    /* translate x: 0, Y: 0 */
    translation = t_operators.classify_translation (reference, t_p0_m0);
    ASSERT_EQ (translation.x, 0);
    ASSERT_EQ (translation.y, 0);

    /* translate not enough points, so T=0,0 */
    translation = t_operators.classify_translation (reference, t_too_few_points);
    ASSERT_EQ (translation.x, 0);
    ASSERT_EQ (translation.y, 0);
}

TEST (scale_translation, classification) {
    // only translation, so the scale should be 1.0
    operators To;
    // clang-format off
    /* reference */
    points_t reference = {
        { 1, { 100, 200 } }, { 2, { 200, 200 } },
        { 3, { 100, 100 } }, { 4, { 200, 100 } }
    };
    /* translate x: -2, Y: +3 */
    points_t t_m2_p3 = {
        { 1, { 98, 203 } }, { 2, { 198, 203 } },
        { 3, { 98, 103 } }, { 4, { 198, 103 } }
    };
    /* translate x: 5, Y: -42 */
    points_t t_p5_m42 = {
        { 1, { 105, 158 } }, { 2, { 205, 158 } },
        { 3, { 105, 58 } }, { 4, { 205, 58 } }
    };
    /* translate x: 0, Y: 0 */
    points_t t_p0_m0 = {
        { 1, { 100, 200 } }, { 2, { 200, 200 } },
        { 3, { 100, 100 } }, { 4, { 200, 100 } }
    };
    /* translate not enough points, so T=0,0 */
    points_t t_too_few_points = {
        { 1, { 100, 200 } }, { 2, { 200, 200 } }
    };
    // clang-format on
    // translations, not scales
    EXPECT_FLOAT_EQ (To.scale (reference, t_m2_p3), 1);
    EXPECT_FLOAT_EQ (To.scale (reference, t_p5_m42), 1);
    EXPECT_FLOAT_EQ (To.scale (reference, t_p0_m0), 1);
    EXPECT_FLOAT_EQ (To.scale (reference, t_too_few_points), 1);
}

TEST (scale, classification) {
    operators To;
    // clang-format off
    /* reference */
    points_t reference = {
        { 1, { 10, 20 } }, { 2, { 20, 20 } },
        { 3, { 10, 10 } }, { 4, { 20, 10 } }
    };
    /* scale x: 0.5, y: 1.0 */
    points_t s_05_1 = {
        { 1, { 10, 20 } }, { 2, { 15, 20 } },
        { 3, { 10, 10 } }, { 4, { 15, 10 } }
    };
    /* scale x: 1.0, y: 0.5 */
    points_t s_1_05 = {
        { 1, { 10, 15 } }, { 2, { 20, 15 } },
        { 3, { 10, 10 } }, { 4, { 20, 10 } }
    };
    /* scale x: 0.5, y: 0.5 */
    points_t s_05_05 = {
        { 1, { 10, 15 } }, { 2, { 15, 15 } },
        { 3, { 10, 10 } }, { 4, { 15, 10 } }
    };
    // clang-format on
    EXPECT_FLOAT_EQ (To.scale (reference, s_05_1), 1);
    EXPECT_FLOAT_EQ (To.scale (reference, s_1_05), 1);
    EXPECT_FLOAT_EQ (To.scale (reference, s_05_05), 0.5);
}
