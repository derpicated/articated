#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include <map>
#include <numeric>
#include <sstream>
#include <vector>

typedef struct keypoint_t {
    float x;
    float y;
} keypoint_t;

typedef struct translation_t {
    float x;
    float y;
} translation_t;

template <typename T> struct kahan_accumulation {
    kahan_accumulation ()
    : sum (0)
    , correction (0) {
    }
    T sum;
    T correction;
};

class operators {
    private:
    const unsigned int _minimum_ref_points = 3;
    const unsigned int _maximum_ref_points = 40;

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
    float classify_scale (const std::map<unsigned int, keypoint_t>& reference_points,
    const std::map<unsigned int, keypoint_t>& data_points);

    /**
     * classify the translation from a number of keypoints to the reference
     * Translation (x, y)
     * @param  marker_points [description]
     * @return               [description]
     */
    translation_t classify_translation (const std::map<unsigned int, keypoint_t>& reference_points,
    const std::map<unsigned int, keypoint_t>& data_points);

    /**
     * classify the rotation from a number of keypoints to the reference
     * Rotation (yaw)
     * @param  marker_points [description]
     * @return               [description]
     */
    float classify_yaw (const std::map<unsigned int, keypoint_t>& reference_points,
    const std::map<unsigned int, keypoint_t>& data_points);

    // angle pitch
    float classify_pitch (const std::map<unsigned int, keypoint_t>& reference_points,
    const std::map<unsigned int, keypoint_t>& data_points);

    // angle roll
    float classify_roll (const std::map<unsigned int, keypoint_t>& reference_points,
    const std::map<unsigned int, keypoint_t>& data_points);

    /**
     * calculate the centroid of a set of points
     * @param  points are the keypoints of the "shape"
     * @return returns the centroid of the points
     */
    keypoint_t centroid (const std::map<unsigned int, keypoint_t>& points);

    /**
     * sums all the values using a kahan accumulation algorithm
     * @param values the values to sum
     * @return returns the sum of the values
     */
    template <typename T> T sum (std::vector<T> values);

    /**
     * sums all the points in a map
     * @param points are the keypoints to sum
     * @return returns the sum of all values
     */
    keypoint_t sum (const std::map<unsigned int, keypoint_t>& points);

    /**
     * calculates the X and Y intersection values
     * this solves these values by using the
     * Y = AX+B formula (line formula)
     * @param  A point A
     * @param  B point B
     * @param  origin origin in these points
     * @return returns the intersection values
     *         [x is intersection on x axis]
     *         [y is intersection on y axis]
     *         returns 0,0 by devision by zero
     */
    keypoint_t intersections (keypoint_t A, keypoint_t B, keypoint_t origin = { 0, 0 });

    /**
     * matches the points to the reference.
     * so that all the points in the marker_points are
     * available in the reference
     * @param marker_points are the reference marker_points
     */
    void match_points (const std::map<unsigned int, keypoint_t>& reference_points,
    std::map<unsigned int, keypoint_t>& data_points);
    /**
     * convert value to an std::string
     * @param value to be converted
     * @return returns a std::string
     */
    template <typename T> std::string to_string (T value);
};

#endif // OPERATORS_HPP
