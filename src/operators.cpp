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

float operators::yaw (const points_t& reference_points, const points_t& data_points) {
    // rotation from top (Z)
    /* match points */
    points_t ref  = reference_points;
    points_t data = data_points;
    match_points (ref, data);
    match_points (data, ref);
    float angle = 0;
    if (data.size () < _minimum_ref_points || data.size () > _maximum_ref_points) {
        return angle;
    }
    // for every point
    //  get delta angle from ref to data
    //  (translate to same origin to use dot product)
    //  sum
    //  take average
    //  and fit into 0-360 degrees range
    /* centroid */
    point_t centroid_r = centroid (ref);
    point_t centroid_d = centroid (data);
    // translate to origin
    translate (ref, { -1 * centroid_r.x, -1 * centroid_r.y });
    translate (data, { -1 * centroid_d.x, -1 * centroid_d.y });

    for (auto point : ref) {
        float product = dot_product_degrees (point.second, data.at (point.first));
        std::cout << "pR(" << point.second.x << "," << point.second.y << ")"
                  << "pD(" << data.at (point.first).x << ","
                  << data.at (point.first).y << ")" << std::endl;
        std::cout << "product: " << product << std::endl;
        // angle += product < 0 ? (360) + product : product;
        angle += product;
    }
    angle /= ref.size ();
    return angle < 0 ? (360) + angle : angle;
}

float operators::pitch (const points_t& reference_points, const points_t& data_points) {
    // y/row
    /* match points */
    points_t ref  = reference_points;
    points_t data = data_points;
    match_points (ref, data);
    match_points (data, ref);
    float angle = 0;
    if (data.size () < _minimum_ref_points || data.size () > _maximum_ref_points) {
        return angle;
    }

    /* centroid */
    point_t centroid_r = centroid (ref);
    /* vertical line */
    const line_t v_ref_line = { { centroid_r.x, centroid_r.y }, { centroid_r.x, 10 } };
    float ref_line_distance  = 0;
    float data_line_distance = 0;
    /**
     * x  +  x
     *    |
     *    o
     *
     * x     x
     * x: point
     * o: centroid
     * |: line
     * +: intersection
     */
    find_matching_intersection (ref, data, v_ref_line, ref_line_distance, data_line_distance);
    // calculate angle
    angle = projected_angle_abs (ref_line_distance, data_line_distance);
    // check direction angle
    // return angle or -1*angle
    return angle;
}

float operators::roll (const points_t& reference_points, const points_t& data_points) {
    // x/coll
    /* match points */
    points_t ref  = reference_points;
    points_t data = data_points;
    match_points (ref, data);
    match_points (data, ref);
    float angle = 0;
    if (data.size () < _minimum_ref_points || data.size () > _maximum_ref_points) {
        return angle;
    }
    /* centroid */
    point_t centroid_r = centroid (ref);

    /* horizontal line */
    const line_t v_ref_line = { { centroid_r.x, centroid_r.y }, { 10, centroid_r.y } };
    float ref_line_distance  = 0;
    float data_line_distance = 0;
    /**
     * x     x
     *
     *    o--+
     *
     * x     x
     * x: point
     * o: centroid
     * |: line
     * +: intersection
     */
    find_matching_intersection (ref, data, v_ref_line, ref_line_distance, data_line_distance);
    // calculate angle
    angle = projected_angle_abs (ref_line_distance, data_line_distance);
    // check direction angle
    // return angle or -1*angle
    return angle;
}

void operators::translate (points_t& points, translation_t T) {
    if (T.x == 0 && T.y == 0) {
        return;
    } else if (T.x == POINT_INF || T.y == POINT_INF) {
        return;
    } else if (std::isnan (T.x) || std::isnan (T.y)) {
        return;
    }
    for (auto point : points) {
        points.at (point.first).x += T.x;
        points.at (point.first).y += T.y;
    }
}

void operators::scale (points_t& points, const float scale) {
    if (!points.size ()) {
        return;
    } else if (scale == POINT_INF) {
        return;
    } else if (std::isnan (scale)) {
        return;
    }

    point_t C = centroid (points);
    // translate to 0,0
    translate (points, { -1 * C.x, -1 * C.y });
    for (auto point : points) {
        points.at (point.first).x *= scale;
        points.at (point.first).y *= scale;
    }
    // translate to centroid
    translate (points, { C.x, C.y });
}

float operators::dot_product (const point_t& p1, const point_t& p2) {
    if ((p1.x == 0 && p1.y == 0) || (p2.x == 0 && p2.y == 0)) {
        return 0;
    } else if (p1.x == POINT_INF || p1.y == POINT_INF || p2.x == POINT_INF || p2.y == POINT_INF) {
        return 0;
    } else if (std::isnan (p1.x) || std::isnan (p1.y) || std::isnan (p2.x) ||
    std::isnan (p2.y)) {
        return 0;
    }
    // angle of p2 relative to p1= atan2(v2.y,v2.x) - atan2(v1.y,v1.x)
    float angle = 0;
    angle       = std::atan2 (p2.y, p2.x) - std::atan2 (p1.y, p1.x);
    if (angle > M_PI) {
        angle -= (2 * M_PI);
    } else if (angle < -M_PI) {
        angle += (2 * M_PI);
    }
    return angle;
}

float operators::dot_product_degrees (const point_t& p1, const point_t& p2) {
    return dot_product (p1, p2) * (180 / M_PI);
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

void operators::find_matching_intersection (const points_t& reference_points,
const points_t& data_points,
const line_t& line,
float& ref_length,
float& data_length) {
    points_t ref  = reference_points;
    points_t data = data_points;
    match_points (ref, data);
    match_points (data, ref);
    if (data.size () < _minimum_ref_points || data.size () > _maximum_ref_points) {
        return;
    }

    /* centroid */
    point_t centroid_r = centroid (ref);
    point_t centroid_d = centroid (data);

    for (auto point1 = ref.begin (); point1 != ref.end (); ++point1) {
        for (auto point2 = ref.begin (); point2 != ref.end (); ++point2) {
            point_t I = intersection (line, { point1->second, point2->second });
            if ((I.x == POINT_INF || I.y == POINT_INF) ||
            (std::isnan (I.x) || std::isnan (I.y)) ||
            (equal (I.x, centroid_r.x) && equal (I.y, centroid_r.y))) {
            } else {
                // check if new ref_line dist is bigger then ref_line
                if (distance ({ centroid_r.x, centroid_r.y }, I) > ref_length) {
                    ref_length = distance ({ centroid_r.x, centroid_r.y }, I);
                    // if so, check data line and set it

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
                    point_t point1_data = data.find (point1->first)->second;
                    point_t point2_data = data.find (point2->first)->second;
                    point_t Idata       = { point1_data.x +
                        ((point2_data.x - point1_data.x) * scale_p1_I),
                        point1_data.y + ((point2_data.y - point1_data.y) * scale_p1_I) };
                    data_length = distance ({ centroid_d.x, centroid_d.y }, Idata);
                }
            }
        }
    }
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
