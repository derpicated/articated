#include "operators.hpp"
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>

/* explicit instantiation declaration */
template float operators::sum<float> (std::vector<float> values);
template double operators::sum<double> (std::vector<double> values);

operators::operators () {
}

operators::~operators () {
}

float operators::classify_scale (const points_t& reference_points, const points_t& data_points) {
    points_t points = data_points;
    float scale     = 1;
    match_points (reference_points, points);
    if (points.size () <= _minimum_ref_points || points.size () > _maximum_ref_points) {
        return scale;
    }
    // centroid
    point_t centroid_r = centroid (reference_points);
    point_t centroid_p = centroid (points);

    // keypoint reference
    keypoint_t ref_A = { reference_points.begin ()->first,
        reference_points.begin ()->second };

    std::map<unsigned int, float> point_ratios; // <point ID, ratio>

    // 1.0 means a perfect 45 degrees through A
    point_t diff        = {};
    const float h_ratio = POINT_INF;
    const float v_ratio = POINT_ZERO;
    for (auto point : reference_points) {
        if (ref_A.id != point.first) {
            diff.x = (point.second.x - ref_A.p.x);
            diff.y = (point.second.y - ref_A.p.y);
            if (diff.y == POINT_ZERO) {
                // horizontal line
                // diff y is 0
                point_ratios.insert ({ point.first, h_ratio });
            } else if (diff.x == POINT_ZERO) {
                // vertical line
                // diff x 0
                point_ratios.insert ({ point.first, v_ratio });
            } else {
                // absolute value, 1.0 is right in between
                point_ratios.insert ({ point.first, std::fabs (diff.x / diff.y) });
            }
        }
    }
    // find optimal vertical and optimal horizontal point
    // find closest to v_ratio
    // find closes to h_ratio
    // horizontal ratio close to 0
    // find closest to optimal_ratio

    unsigned int h_ratio_optimal = point_ratios.begin ()->first;
    unsigned int v_ratio_optimal = point_ratios.begin ()->first;
    for (auto ratio : point_ratios) {
        h_ratio_optimal = std::fabs (ratio.second - h_ratio) <
        std::fabs (point_ratios.find (h_ratio_optimal)->second - h_ratio) ?
        ratio.first :
        h_ratio_optimal;
        v_ratio_optimal = std::fabs (ratio.second - v_ratio) <
        std::fabs (point_ratios.find (v_ratio_optimal)->second - v_ratio) ?
        ratio.first :
        v_ratio_optimal;
    }
    // keypoint to use for x and y intersections
    keypoint_t ref_x = { h_ratio_optimal,
        reference_points.find (h_ratio_optimal)->second };
    keypoint_t ref_y = { v_ratio_optimal,
        reference_points.find (v_ratio_optimal)->second };
    // keypoint_t ref_B = { optimal_point, reference_points.find
    // (optimal_point)->second };

    std::cout << "optimal x: " << ref_x.id << std::endl;
    std::cout << "optimal y: " << ref_y.id << std::endl;
    // find intersections
    // point_t intersection_ref   = intersections (ref_A.p, ref_B.p,
    // centroid_r);
    // point_t intersection_ref_x = { intersection_ref.x, 0 };
    // point_t intersection_ref_y = { 0, intersection_ref.y };
    // ratio of intersection
    // <------->
    // <--->
    // A   X   B
    // X
    // calculate vector
    // Y
    float ratio_ref_a_x_b;
    float ratio_ref_a_y_b;

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

    return scale;
}

translation_t operators::classify_translation (const points_t& reference_points,
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

point_t operators::intersections (point_t p1, point_t p2, point_t origin) {
    point_t I = { 0, 0 }; // intersection
    // normalize using the origin
    p1 = { p1.x - origin.x, p1.y - origin.y };
    p2 = { p2.x - origin.x, p2.y - origin.y };
    // Y = AX + B
    float dY = p2.y - p1.y;
    float dX = p2.x - p1.x;
    // A
    float A;
    // horizontal line,
    // dY == 0 and p1.y == 0, x = 0
    // else x = inf
    if (dY == POINT_ZERO) {
        I.y = p1.y;
        if (p1.y == POINT_ZERO) {
            I.x = POINT_ZERO;
        } else {
            I.x = POINT_INF;
        }
        return I;
    }
    // vertical line
    // dX == 0 and pi.x == 0, y = 0
    // else Y = inf
    if (dX == POINT_ZERO) {
        I.x = p1.x;
        if (p1.x == POINT_ZERO) {
            I.y = POINT_ZERO;
        } else {
            I.y = POINT_INF;
        }
        return I;
    }
    A = dY / dX;
    // B
    float B = p1.y - A * p1.x;
    // intersection X axis; Y = 0
    // x = (y - B) / A
    I.x = (0 - B) / A;
    // intersection Y axis; X = 0
    // y = Ax+b = b
    I.y = B;
    I   = { I.x, I.y };
    return I;
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
