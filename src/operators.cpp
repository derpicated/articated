#include "operators.hpp"
#include <iostream>
#include <sstream>
#include <string>


operators::operators () {
}

operators::~operators () {
}

float classify_scale (const std::map<unsigned int, keypoint_t>& marker_points) {
    return 0;
}

float classify_translation (const std::map<unsigned int, keypoint_t>& marker_points) {
    return 0;
}

float classify_yaw (const std::map<unsigned int, keypoint_t>& marker_points) {
    return 0;
}

float classify_pitch (const std::map<unsigned int, keypoint_t>& marker_points) {
    return 0;
}

float classify_roll (const std::map<unsigned int, keypoint_t>& marker_points) {
    return 0;
}

keypoint_t operators::calculate_centroid (const std::map<unsigned int, keypoint_t>& points) {
    keypoint_t centroid = { 0, 0 };
    // 40 sums of unsigned short ints will not go over this limit
    long unsigned int keypoint_sum_x;
    long unsigned int keypoint_sum_y;
    if (points.size () && points.size () <= _maximum_ref_points) {
        keypoint_sum_x = 0;
        keypoint_sum_y = 0;
        for (auto point : points) {
            keypoint_sum_x += point.second.x;
            keypoint_sum_y += point.second.y;
        }
        centroid.x = keypoint_sum_x / points.size ();
        centroid.y = keypoint_sum_y / points.size ();
    }
    return centroid;
}

void operators::set_reference (const std::map<unsigned int, keypoint_t>& marker_points) {
    if (marker_points.size () <= _minimum_ref_points) {
        throw std::length_error ("too few reference points; found " +
        to_string (marker_points.size ()) + " need " + to_string (_minimum_ref_points));
    }
    _reference_markers  = marker_points;
    _reference_centroid = calculate_centroid (_reference_markers);
}

    }
}

template <typename T> std::string operators::to_string (T value) {
    std::ostringstream os;
    os << value;
    return os.str ();
}
