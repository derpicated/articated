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

float operators::classify_scale (const std::map<unsigned int, keypoint_t>& marker_points) {
    auto points = marker_points;
    match_to_reference (points);
    if (points.size () <= _minimum_ref_points) {
        return 1;
    }

    return 1;
}

translation_t operators::classify_translation (
const std::map<unsigned int, keypoint_t>& marker_points) {
    translation_t translation = { 0, 0 };
    auto points               = marker_points;
    match_to_reference (points);
    if (points.size () <= _minimum_ref_points) {
        return translation;
    }
    keypoint_t centroid_points = centroid (points);
    translation.x              = centroid_points.x - _reference_centroid.x;
    translation.y              = centroid_points.y - _reference_centroid.y;
    return translation;
}

float operators::classify_yaw (const std::map<unsigned int, keypoint_t>& marker_points) {
    return 0;
}

float operators::classify_pitch (const std::map<unsigned int, keypoint_t>& marker_points) {
    return 0;
}

float operators::classify_roll (const std::map<unsigned int, keypoint_t>& marker_points) {
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

void operators::set_reference (const std::map<unsigned int, keypoint_t>& marker_points) {
    if (marker_points.size () <= _minimum_ref_points) {
        throw std::length_error ("too few reference points; found " +
        to_string (marker_points.size ()) + " need " + to_string (_minimum_ref_points));
    }
    _reference_markers  = marker_points;
    _reference_centroid = centroid (_reference_markers);
}

void operators::match_to_reference (std::map<unsigned int, keypoint_t>& marker_points) {
    for (auto marker = marker_points.begin (); marker != marker_points.end ();) {
        if (_reference_markers.find (marker->first) == _reference_markers.end ()) {
            marker = marker_points.erase (marker);
        } else {
            ++marker;
        }
    }
}

template <typename T> std::string operators::to_string (T value) {
    std::ostringstream os;
    os << value;
    return os.str ();
}
