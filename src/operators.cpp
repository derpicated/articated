#include "operators.hpp"
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <stdint.h>
#include <string>

/* explicit instantiation declaration */
template float operators::sum<float> (std::vector<float> values);
template double operators::sum<double> (std::vector<double> values);

operators::operators () {
}

operators::~operators () {
}

float operators::scale (const points_t& reference_points,
const points_t& data_points,
unsigned int granularity) {
    points_t points     = data_points;
    points_t ref_points = reference_points;
    float scale         = 1;
    match_points (reference_points, points);
    match_points (points, ref_points);
    if (points.size () < _minimum_ref_points || points.size () > _maximum_ref_points) {
        return scale;
    }
    granularity = (granularity < 1) ? 2 : granularity;

    /* centroid */
    point_t centroid_r = centroid (ref_points);
    point_t centroid_d = centroid (points);

    const auto r_line_length = 10;
    line_t ref_line          = { { 0, 0 }, { r_line_length, 0 } };
    float radians;
    std::vector<float> scales = {};
    for (unsigned int step = 0; step < granularity; step++) {
        // find a point from the centroid at x*rps
        ref_line.p1.x = centroid_r.x;
        ref_line.p1.y = centroid_r.y;
        radians       = (M_PI * step) / (granularity - 1);
        ref_line.p2.x = centroid_r.x + (std::cos (radians) * r_line_length);
        ref_line.p2.y = centroid_r.y + (std::sin (radians) * r_line_length);
        // find an intersection
        // fail if:
        // - x or y is inf (parallel)
        // - NaN
        // - intersection at centroid
        for (auto point1 = ref_points.begin (); point1 != ref_points.end (); ++point1) {
            for (auto point2 = ref_points.begin (); point2 != ref_points.end (); ++point2) {
                point_t I = intersection (ref_line, { point1->second, point2->second });
                if ((I.x == POINT_INF || I.y == POINT_INF) ||
                (std::isnan (I.x) || std::isnan (I.y)) ||
                (equal (I.x, centroid_r.x) && equal (I.y, centroid_r.y))) {
                } else {
                    // found intersection
                    //   length of centroid to intersection
                    // point1 point2
                    // calculate scale from
                    //   point1 -> point2
                    //   point1 -> intersection
                    // find point in data using scale and markers
                    // calculate length from centroid data to point
                    //
                    // scale from length ref_data to length data
                    // that is scale
                    // add to scales vector
                    // ref centroid to intersection
                    float length_rc_i = distance (centroid_r, I);

                    // scale p1 to I (using p1 and p2)
                    float dist_p1_p2 = distance (point1->second, point2->second);
                    float dist_p1_I  = distance (point1->second, I);
                    // scale
                    float scale_p1_I = dist_p1_I / dist_p1_p2;
                    // scale pos or neg
                    if (scale_p1_I) { // not zero
                        // [-scale]  p1  [+scale]  p2  [+scale]
                        if (is_in_front ({ point1->second, point2->second },
                            point1->second, I)) {
                            scale_p1_I *= -1;
                        }
                    }
                    // find point in data
                    point_t point1_data = points.find (point1->first)->second;
                    point_t point2_data = points.find (point2->first)->second;
                    point_t Idata       = { point1_data.x +
                        ((point2_data.x - point1_data.x) * scale_p1_I),
                        point1_data.y + ((point2_data.y - point1_data.y) * scale_p1_I) };
                    scales.push_back (distance (centroid_d, Idata) / length_rc_i);
                    // end the loop
                    point1 = --ref_points.end ();
                    point2 = --ref_points.end ();
                }
            };
        };
    }
    // check scales and find closes to 1
    if (!scales.empty ()) {
        return closest (scales, scale);
    } else {
        return scale;
    }
    // clang-format off
    // /**
    //  * find points that go through x, and y axis in reference
    //  * p_s = start point
    //  * p_x = point that makes a line through x axis to p_start
    //  * p_y = point that makes a line through y axis to p_start
    //  * [p is point, r is reference, _? = variable]
    //  */
    // keypoint_t pr_s = { reference_points.begin ()->first,
    //     reference_points.begin ()->second };
    // keypoint_t pr_x;
    // keypoint_t pr_y;
    // /**
    //  * find the ratio between the delta x and delta y
    //  * from point pr_start to all points
    //  *        ratio of | means
    //  *             1.0 | right in between
    //  * POINT_INF (inf) | horizontal line
    //  *  POINT_ZERO (0) | vertical line
    //  */
    // std::map<unsigned int, float> point_ratios; // <point ID, ratio>
    // const float h_ratio = POINT_INF;
    // const float v_ratio = POINT_ZERO;
    // point_t delta       = {};
    // for (auto point : reference_points) {
    //     if (pr_s.id != point.first) {
    //         delta.x = (point.second.x - pr_s.p.x);
    //         delta.y = (point.second.y - pr_s.p.y);
    //         if (delta.y == POINT_ZERO) {
    //             // horizontal line
    //             // diff y is 0
    //             point_ratios.insert ({ point.first, h_ratio });
    //         } else if (delta.x == POINT_ZERO) {
    //             // vertical line
    //             // diff x 0
    //             point_ratios.insert ({ point.first, v_ratio });
    //         } else {
    //             // absolute value, 1.0 is right in between
    //             point_ratios.insert ({ point.first, std::fabs (delta.x / delta.y) });
    //         }
    //     }
    // }
    // /**
    //  * find optimal value for a horizontal line (will go through the Y-axis)
    //  * and one for a vertical line (will go through the X-axis)
    //  */
    // /* keys for reference points */
    // unsigned int h_ratio_optimal_p = point_ratios.begin ()->first;
    // unsigned int v_ratio_optimal_p = point_ratios.begin ()->first;
    // for (auto ratio : point_ratios) {
    //     h_ratio_optimal_p = std::fabs (ratio.second - h_ratio) <
    //     std::fabs (point_ratios.find (h_ratio_optimal_p)->second - h_ratio) ?
    //     ratio.first :
    //     h_ratio_optimal_p;
    //     v_ratio_optimal_p = std::fabs (ratio.second - v_ratio) <
    //     std::fabs (point_ratios.find (v_ratio_optimal_p)->second - v_ratio) ?
    //     ratio.first :
    //     v_ratio_optimal_p;
    // }
    // pr_x = { v_ratio_optimal_p, reference_points.find (v_ratio_optimal_p)->second };
    // pr_y = { h_ratio_optimal_p, reference_points.find (h_ratio_optimal_p)->second };
    //
    // /* intersections through x and y axis on reference */
    // point_t pr_sx_intersection = intersections (pr_s.p, pr_x.p, centroid_r);
    // pr_sx_intersection.y       = 0;
    // // pr_sx_intersection         = { pr_sx_intersection.x - centroid_r.x,
    // //     pr_sx_intersection.y - centroid_r.y };
    // point_t pr_sy_intersection = intersections (pr_s.p, pr_y.p, centroid_r);
    // pr_sy_intersection.x       = 0;
    // // pr_sy_intersection         = { pr_sy_intersection.x - centroid_r.x,
    // //     pr_sy_intersection.y - centroid_r.y };
    //
    // // - for pr_s tot pr_x
    // // - for pr_s to pr_y
    // // find ratio on where the intersection is
    // // ps_s <-------> ps_x
    // // ps_s <---> intersection
    // //
    // // find the three points in the data
    // // using this scale find the intersection in the data
    // //
    // // calculate reference and data origin to intersection length
    // // calculate scale from ref to data
    // // for pr_s to pr_y
    // // smallest scale is the scale?
    //
    // /**
    //  * find scale
    //  * ps_s <-------> ps_x
    //  * ps_s <---> intersection
    //  */
    // float dist_pr_sx  = distance (pr_s.p, pr_x.p);
    // float dist_pr_sxi = distance (
    // { pr_s.p.x - centroid_r.x, pr_s.p.y - centroid_r.y }, pr_sx_intersection);
    // float scale_pr_sx = dist_pr_sxi / dist_pr_sx;
    // float dist_pr_sy  = distance (pr_s.p, pr_y.p);
    // float dist_pr_syi = distance (
    // { pr_s.p.x - centroid_r.x, pr_s.p.y - centroid_r.y }, pr_sy_intersection);
    // float scale_pr_sy = dist_pr_syi / dist_pr_sy;
    //
    // /**
    //  * find intersections in data
    //  */
    // keypoint_t pd_s = { data_points.find (pr_s.id)->first,
    //     data_points.find (pr_s.id)->second };
    // keypoint_t pd_x = { data_points.find (pr_x.id)->first,
    //     data_points.find (pr_x.id)->second };
    // keypoint_t pd_y = { data_points.find (pr_y.id)->first,
    //     data_points.find (pr_y.id)->second };
    //
    // point_t pd_sx_intersection = { pd_s.p.x + ((pd_x.p.x - pd_s.p.x) * scale_pr_sx),
    //     pd_s.p.y + ((pd_x.p.y - pd_s.p.y) * scale_pr_sx) };
    // pd_sx_intersection = { pd_sx_intersection.x - centroid_d.x,
    //     pd_sx_intersection.y - centroid_d.y };
    //
    // point_t pd_sy_intersection = { pd_s.p.x + ((pd_y.p.x - pd_s.p.x) * scale_pr_sy),
    //     pd_s.p.y + ((pd_y.p.y - pd_s.p.y) * scale_pr_sy) };
    // pd_sy_intersection = { pd_sy_intersection.x - centroid_d.x,
    //     pd_sy_intersection.y - centroid_d.y };

    // ratio of intersection
    // <------->
    // <--->
    // A   X   B
    // X
    // calculate vector
    // Y


    // keypoint to use for x and y intersections on data
    // keypoint_t data_x = { h_ratio_optimal, data_points.find
    // (h_ratio_optimal)->second };
    // keypoint_t data_y = { v_ratio_optimal, data_points.find
    // (v_ratio_optimal)->second };

    // [from reference]
    // find line(set of 2 points) that crosses x and y axis at some point
    // make sure that this line is as close to 45 degrees to the centroid
    // so that an overflow isn't bound to happen due to very large values
    //
    // with these points, solve Y=AX+B
    // (intersection points with X and Y axis)
    // where the centroid is the origin
    //
    // note the ratio where the intersection lays
    // on the line between the two points
    //
    // [from data]
    // using the two points and the ratio, look for the x/y intersection
    // and compare the vectors from origin to intersections
    // The one wit the difference can be used to calculate the scale
    // (new size/old size)
    //
    // for reference
    // pick always first point (let's call it A)
    //  for all points
    //  calculate the mean x, y difference from point A
    //  pick point with highest mean difference
    //  solve Y=AX+B
    //  solve Y=0
    //   note ratio of vectors between points
    //  and X=0
    //   note ratio of vectors between points
    //
    // calculate x, y location of x axis
    // calculate x, y location of y axis
    //
    // calculate vector x axis
    // calculate vector y axis
    //
    // line with smallest difference can be used for scale
    // clang-format on
    return scale;
}

translation_t operators::translation (const points_t& reference_points,
const points_t& data_points) {
    translation_t translation = { 0, 0 };
    points_t points           = data_points;
    match_points (reference_points, points);
    if (points.size () <= _minimum_ref_points) {
        return translation;
    }
    point_t centroid_reference = centroid (reference_points);
    point_t centroid_points    = centroid (points);
    translation.x              = centroid_points.x - centroid_reference.x;
    translation.y              = centroid_points.y - centroid_reference.y;
    return translation;
}

float operators::classify_yaw (const points_t& reference_points, const points_t& data_points) {
    return 0;
}

float operators::classify_pitch (const points_t& reference_points, const points_t& data_points) {
    return 0;
}

float operators::classify_roll (const points_t& reference_points, const points_t& data_points) {
    return 0;
}

float operators::projected_angle_abs (const float R, const float D) {
    float angle = 0; // radians
    if (equal (R, 0) || equal (D, 0)) {
        angle = 0;
    } else if (R > D) {
        angle = std::acos (D / R);
    } else if (R < D) {
        angle = std::acos (R / D);
    } else {
        angle = 0;
    }
    return angle * (180 / M_PI); // to degrees
}

point_t operators::centroid (const points_t& points) {
    point_t centroid = { 0, 0 };
    if (points.size () && points.size () <= _maximum_ref_points) {
        centroid = sum (points);
        centroid.x /= points.size ();
        centroid.y /= points.size ();
    }
    return centroid;
}

template <typename T> T operators::sum (std::vector<T> values) {
    kahan_accumulation<T> init;
    kahan_accumulation<T> result = std::accumulate (values.begin (),
    values.end (), init, [](kahan_accumulation<T> accumulation, T value) {
        kahan_accumulation<T> result;
        T y               = value - accumulation.correction;
        T t               = accumulation.sum + y;
        result.correction = (t - accumulation.sum) - y;
        result.sum        = t;
        return result;
    });
    return result.sum;
}

point_t operators::sum (const points_t& points) {
    point_t keypoint_sum     = {};
    std::vector<float> vec_x = {};
    std::vector<float> vec_y = {};
    for (auto point : points) {
        vec_x.push_back (point.second.x);
        vec_y.push_back (point.second.y);
    }
    keypoint_sum.x = sum<float> (vec_x);
    keypoint_sum.y = sum<float> (vec_y);
    return keypoint_sum;
}

float operators::closest (const std::vector<float>& vec, float compare) {
    if (vec.empty ()) {
        return 0;
    }
    float best = vec.front ();
    for (auto val : vec) {
        if (std::fabs (compare - val) <= std::fabs (compare - best)) {
            best = val;
        }
    }
    return best;
}

bool operators::is_in_front (line_t L, point_t R, point_t P) {
    if (a (L) != POINT_INF) { // not vertical, use x
        if (L.p1.x < L.p2.x) {
            // P1:low -> P2:high
            if (P.x < R.x) {
                return true;
            } else {
                return false;
            }
        } else if (L.p1.x > L.p2.x) {
            // P1:high -> P2:low
            if (P.x > R.x) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else if (a (L) != POINT_ZERO) { // not horizontal, use y
        if (L.p1.y < L.p2.y) {
            // P1:low -> P2:high
            if (P.y < R.y) {
                return true;
            } else {
                return false;
            }
        } else if (L.p1.y > L.p2.y) {
            // P1:high -> P2:low
            if (P.y > R.y) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool operators::equal (float a, float b, float error) {
    if (std::isnan (a) | std::isnan (b)) {
        // NaN vs NaN is not equal
        return false;
    } else if (a == POINT_INF && b == POINT_INF) {
        // INF vs INF is equal
        return true;
    } else if (a == POINT_INF || b == POINT_INF) {
        // INF vs ... is not equal
        return false;
    } else {
        return almost_equal_rel_abs (a, b, error);
    }
}

/* See
https://randomascii.wordpress.com/2012/01/11/tricks-with-the-floating-point-format/
for the potential portability problems with the union and bit-fields below.
*/
union Float_t {
    Float_t (float num = 0.0f)
    : f (num) {
    }
    // Portable extraction of components.
    bool Negative () const {
        return i < 0;
    }
    int32_t RawMantissa () const {
        return i & ((1 << 23) - 1);
    }
    int32_t RawExponent () const {
        return (i >> 23) & 0xFF;
    }
    int32_t i;
    float f;
#ifdef _DEBUG
    struct { // Bitfields for exploration. Do not use in production code.
        uint32_t mantissa : 23;
        uint32_t exponent : 8;
        uint32_t sign : 1;
    } parts;
#endif
};

bool operators::almost_equal_ulp_abs (float A, float B, float maxDiff, int maxUlpsDiff) {
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    float absDiff = std::fabs (A - B);
    if (absDiff <= maxDiff) return true;

    Float_t uA (A);
    Float_t uB (B);

    // Different signs means they do not match.
    if (uA.Negative () != uB.Negative ()) return false;

    // Find the difference in ULPs.
    int ulpsDiff = std::abs (uA.i - uB.i);
    if (ulpsDiff <= maxUlpsDiff) return true;

    return false;
}

bool operators::almost_equal_rel_abs (float A, float B, float maxDiff, float maxRelDiff) {
    // Check if the numbers are really close -- needed
    // when comparing numbers near zero.
    float diff = std::fabs (A - B);
    if (diff <= maxDiff) return true;

    A             = std::fabs (A);
    B             = std::fabs (B);
    float largest = (B > A) ? B : A;

    if (diff <= largest * maxRelDiff) return true;
    return false;
}

point_t operators::intersections (point_t p1, point_t p2, point_t origin) {
    point_t I = { 0, 0 }; // intersection
    // normalize using the origin
    p1 = { p1.x - origin.x, p1.y - origin.y };
    p2 = { p2.x - origin.x, p2.y - origin.y };
    // Y = AX + B
    float dY = p2.y - p1.y;
    float dX = p2.x - p1.x;
    // A
    float A = a ({ p1, p2 });
    // horizontal line,
    if (equal (A, POINT_ZERO)) {
        I.y = p1.y;
        I.x = POINT_INF;
        return I;
    }
    // vertical line
    if (A == POINT_INF) {
        I.x = p1.x;
        I.y = POINT_INF;
        return I;
    }
    A = dY / dX;
    // B
    float B = b ({ p1, p2 });
    // intersection X axis; Y = 0
    // x = (y - B) / A
    I.x = (0 - B) / A;
    // intersection Y axis; X = 0
    // y = Ax+b = b
    I.y = B;
    return I;
}

point_t operators::intersection (line_t l1, line_t l2) {
    point_t I;
    // one or more A is INF, intersection is INF
    // line slope NaN?
    if (equal (a (l1), a (l2))) { // parallel
        I = { POINT_INF, POINT_INF };
    } else if (a (l1) == POINT_INF) {     // l1 vertical line
        I.x = l1.p1.x;                    // x = any x point
        if (equal (a (l2), POINT_ZERO)) { // l2 horizontal line
            I.y = l2.p1.y;
        } else { // l2 normal line
            I.y = y (I.x, l2);
        }
    } else if (a (l2) == POINT_INF) {     // l2 vertical line
        I.x = l2.p1.x;                    // x = any x point
        if (equal (a (l1), POINT_ZERO)) { // l1 horizontal line
            I.y = l1.p1.y;
        } else { // l1 normal line
            I.y = y (I.x, l1);
        }
    } else if (equal (a (l1), POINT_ZERO)) { // l1 horizontal line
        I.y = l1.p1.y;
        I.x = x (I.y, l2);
    } else if (equal (a (l2), POINT_ZERO)) { // l2 horizontal line
        I.y = l2.p1.y;
        I.x = x (I.y, l1);
    } else {
        // (B2 - B1)/(A1 - A2)
        // no devision by zero because A's wont be the same
        I.x = (b (l2) - b (l1)) / (a (l1) - a (l2));
        I.y = y (I.x, l1);
    }
    return I;
}

float operators::a (line_t line) {
    // check points for NaN or INF
    if ((line.p1.x == POINT_INF || line.p1.y == POINT_INF) ||
    (line.p2.x == POINT_INF || line.p2.y == POINT_INF)) {
        return NAN;
    } else if ((std::isnan (line.p1.x) || std::isnan (line.p1.y)) ||
    (std::isnan (line.p2.x) || std::isnan (line.p2.y))) {
        return NAN;
    }
    float dY = line.p2.y - line.p1.y;
    float dX = line.p2.x - line.p1.x;
    // dot; dY == 0 and dX == 0
    if (dY == POINT_ZERO && dX == POINT_ZERO) {
        return NAN; // a dot?
    }
    // infinity plane; dY == INF and dX == INF
    if (dY == POINT_INF && dX == POINT_INF) {
        return NAN; // all directions?
    }
    // horizontal line,
    // dY == 0
    if (equal (dY, POINT_ZERO)) {
        return POINT_ZERO;
    }
    // vertical line
    // dX == 0
    if (equal (dX, POINT_ZERO)) {
        return POINT_INF;
    }
    return dY / dX;
}

float operators::b (line_t line) {
    float A = a (line);
    // horizontal line,
    // A == 0
    if (equal (A, POINT_ZERO)) {
        return line.p1.y;
    }
    // vertical line
    // A == 0, B = INF
    if (A == POINT_INF) {
        return POINT_INF;
    }
    // NaN
    if (std::isnan (A)) {
        return NAN;
    }
    // B
    return line.p1.y - A * line.p1.x;
}

float operators::x (float y, line_t line) {
    float A = a (line);
    float B = b (line);
    if (equal (A, POINT_ZERO) && B != POINT_INF) {
        return POINT_INF; // horizontal line, X = INF
    } else if (A == POINT_INF || B == POINT_INF) {
        return line.p1.x; // vertical line, X = point x
    } else {
        return (y - B) / A;
    }
}

float operators::y (float x, line_t line) {
    float A = a (line);
    float B = b (line);
    if (equal (A, POINT_ZERO) && B != POINT_INF) {
        return line.p1.y; // horizontal line, Y = point y
    } else if (A == POINT_INF || B == POINT_INF) {
        return POINT_INF; // vertical line, Y = INF
    } else {
        return A * x + B;
    }
}

float operators::distance (point_t A, point_t B) {
    point_t d = { std::fabs (B.x - A.x), std::fabs (B.y - A.y) };
    return std::sqrt (d.x * d.x + d.y * d.y);
}

void operators::match_points (const points_t& reference_points, points_t& data_points) {
    for (auto point = data_points.begin (); point != data_points.end ();) {
        if (reference_points.find (point->first) == reference_points.end ()) {
            point = data_points.erase (point);
        } else {
            ++point;
        }
    }
}

template <typename T> std::string operators::to_string (T value) {
    std::ostringstream os;
    os << value;
    return os.str ();
}
