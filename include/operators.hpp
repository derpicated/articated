#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include <map>
#include <sstream>

typedef struct keypoint_t {
    unsigned int x;
    unsigned int y;
} keypoint_t;

class operators {
    private:
    std::map<unsigned int, keypoint_t> _reference_markers = {};
    const unsigned int _minimal_ref_points = 3;

    public:
    operators ();
    ~operators ();


    /**
     * Quaternion
     * Z [↻ Yaw ψ]  X [⟲ Roll φ]
     * ^           ^
     * |         /
     * |       /
     * |     /
     * |   /
     * | /
     * +--------> Y [⟲ Pitch θ]
     */

    /**
     * classify the scale from a number of keypoints to the reference
     * Scale (z)
     * @param  marker_points [description]
     * @return               [description]
     */
    float classify_scale (const std::map<unsigned int, keypoint_t>& marker_points);

    /**
     * classify the translation from a number of keypoints to the reference
     * Translation (x, y)
     * @param  marker_points [description]
     * @return               [description]
     */
    float classify_translation (const std::map<unsigned int, keypoint_t>& marker_points);

    /**
     * classify the rotation from a number of keypoints to the reference
     * Rotation (yaw)
     * @param  marker_points [description]
     * @return               [description]
     */
    float classify_yaw (const std::map<unsigned int, keypoint_t>& marker_points);

    // angle pitch
    float classify_pitch (const std::map<unsigned int, keypoint_t>& marker_points);

    // angle roll
    float classify_roll (const std::map<unsigned int, keypoint_t>& marker_points);

    /**
     * set the reference from key points
     * @param marker_points are the reference marker_points
     * @return returns the reference marker_points
     */
    std::map<unsigned int, keypoint_t> set_reference (
    const std::map<unsigned int, keypoint_t>& marker_points);

    /**
     * convert value to an std::string
     * @param value to be converted
     * @return returns a std::string
     */
    template <typename T> std::string to_string (T value);
};

#endif // OPERATORS_HPP
