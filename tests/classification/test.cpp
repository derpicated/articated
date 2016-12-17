#include "operators.hpp"
#include <gtest/gtest.h>

TEST (classify_translation, classification) {
    operators t_operators;
    // clang-format off
    /* reference */
    std::map<unsigned int, keypoint_t> reference = {
        { 1, { 100, 200 } }, { 2, { 200, 200 } },
        { 3, { 100, 100 } }, { 4, { 200, 100 } }
    };
    /* translate x: -2, Y: +3 */
    std::map<unsigned int, keypoint_t> t_m2_p3 = {
        { 1, { 98, 203 } }, { 2, { 198, 203 } },
        { 3, { 98, 103 } }, { 4, { 198, 103 } }
    };
    /* translate x: 5, Y: -42 */
    std::map<unsigned int, keypoint_t> t_p5_m42 = {
        { 1, { 105, 158 } }, { 2, { 205, 158 } },
        { 3, { 105, 58 } }, { 4, { 205, 58 } }
    };
    /* translate x: 0, Y: 0 */
    std::map<unsigned int, keypoint_t> t_p0_m0 = {
        { 1, { 100, 200 } }, { 2, { 200, 200 } },
        { 3, { 100, 100 } }, { 4, { 200, 100 } }
    };
    /* translate not enough points, so T=0,0 */
    std::map<unsigned int, keypoint_t> t_too_few_points = {
        { 1, { 100, 200 } }, { 2, { 200, 200 } }
    };
    // clang-format on
    t_operators.set_reference (reference);
    translation_t translation = { 0, 0 };

    /* translate x: -2, Y: +3 */
    translation = t_operators.classify_translation (t_m2_p3);
    ASSERT_EQ (translation.x, -2);
    ASSERT_EQ (translation.y, +3);

    /* translate x: 5, Y: -42 */
    translation = t_operators.classify_translation (t_p5_m42);
    ASSERT_EQ (translation.x, +5);
    ASSERT_EQ (translation.y, -42);

    /* translate x: 0, Y: 0 */
    translation = t_operators.classify_translation (t_p0_m0);
    ASSERT_EQ (translation.x, 0);
    ASSERT_EQ (translation.y, 0);

    /* translate not enough points, so T=0,0 */
    translation = t_operators.classify_translation (t_too_few_points);
    ASSERT_EQ (translation.x, 0);
    ASSERT_EQ (translation.y, 0);
}
