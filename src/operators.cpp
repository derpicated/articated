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


std::map<unsigned int, keypoint_t> operators::set_reference (
const std::map<unsigned int, keypoint_t>& marker_points) {
    if (marker_points.size () < _minimal_ref_points) {
        throw std::length_error ("too few reference points found " +
        to_string (marker_points.size ()) + " need " + to_string (_minimal_ref_points));
    }
    _reference_markers = marker_points;
    return _reference_markers;
}

template <typename T> std::string operators::to_string (T value) {
    std::ostringstream os;
    os << value;
    return os.str ();
}
