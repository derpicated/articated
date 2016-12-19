#include "operators.hpp"
#include <iostream>
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

float operators::classify_scale (const std::map<unsigned int, keypoint_t>& reference_points,
const std::map<unsigned int, keypoint_t>& data_points) {
    // auto points = marker_points;
    float scale = 1;
    // match_points (points);
    // if (points.size () <= _minimum_ref_points) {
    //     return scale;
    // }
    // centroid
    // keypoint_t p_centroid = centroid (points);
    // std::map<unsigned int, float> mean_translations;
    // keypoint_t A = _reference_markers.begin ()->second;
    // float mean_diff;
    // for (auto point : _reference_markers) {
    //     mean_diff = (point.second.x - A.x) + (point.second.y - A.y);
    //     mean_diff /= 2;
    //     mean_translations.insert ({ point.first, mean_diff });
    //     std::cout << "point: " << point.first << " diff: " << mean_diff <<
    //     std::endl;
    // }
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

translation_t operators::classify_translation (
const std::map<unsigned int, keypoint_t>& reference_points,
const std::map<unsigned int, keypoint_t>& data_points) {
    translation_t translation = { 0, 0 };
    std::map<unsigned int, keypoint_t> points = data_points;
    match_points (reference_points, points);
    if (points.size () <= _minimum_ref_points) {
        return translation;
    }
    keypoint_t centroid_reference = centroid (reference_points);
    keypoint_t centroid_points    = centroid (points);
    translation.x                 = centroid_points.x - centroid_reference.x;
    translation.y                 = centroid_points.y - centroid_reference.y;
    return translation;
}

float operators::classify_yaw (const std::map<unsigned int, keypoint_t>& reference_points,
const std::map<unsigned int, keypoint_t>& data_points) {
    return 0;
}

float operators::classify_pitch (const std::map<unsigned int, keypoint_t>& reference_points,
const std::map<unsigned int, keypoint_t>& data_points) {
    return 0;
}

float operators::classify_roll (const std::map<unsigned int, keypoint_t>& reference_points,
const std::map<unsigned int, keypoint_t>& data_points) {
    return 0;
}

keypoint_t operators::centroid (const std::map<unsigned int, keypoint_t>& points) {
    keypoint_t centroid = { 0, 0 };
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

keypoint_t operators::sum (const std::map<unsigned int, keypoint_t>& points) {
    keypoint_t keypoint_sum  = {};
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

keypoint_t operators::intersections (keypoint_t p1, keypoint_t p2, keypoint_t origin) {
    keypoint_t I = { 0, 0 }; // intersection
    // normalize using the origin
    // p1 = { p1.x - origin.x, p1.y - origin.y };
    // p2 = { p2.x - origin.x, p2.y - origin.y };
    // Y = AX + B
    float At = p2.y - p1.y;
    float Ab = p2.x - p1.x;
    // A
    float A;
    try {
        if (Ab == 0) {
            throw std::overflow_error ("Divide by zero exception");
        }
        A = At / Ab;
        if (A == 0) {
            throw std::overflow_error ("Never crosses X axes");
        }
    } catch (std::overflow_error) {
        return I;
    }
    // B
    float B = p1.y - A * p1.x;
    // intersection X axis; Y = 0
    // x = (y - B) / A
    I.x = (0 - B) / A;
    // intersection Y axis; X = 0
    // y = Ax+b = b
    I.y = B;
    // de-normalize using the origin
    I = { I.x - origin.x, I.y - origin.y };
    return I;
}

void operators::match_points (const std::map<unsigned int, keypoint_t>& reference_points,
std::map<unsigned int, keypoint_t>& data_points) {
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
