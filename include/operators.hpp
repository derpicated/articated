#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <vector>

/**
* point is an arbitrary interesting point
*/
typedef struct point_t {
    float x;
    float y;
} point_t;
/**
 * keypoint is an arbitrary interesting point
 * that has some identifier
 */
typedef struct keypoint_t {
    unsigned int id;
    point_t p;
} keypoint_t;

/**
 * A line
 */
typedef struct line_t {
    point_t p1;
    point_t p2;
} line_t;

typedef std::map<unsigned int, point_t> points_t;

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

const float POINT_MIN  = std::numeric_limits<float>::min ();
const float POINT_ZERO = 0;
const float POINT_MAX  = std::numeric_limits<float>::max ();
const float POINT_INF  = std::numeric_limits<float>::infinity ();

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
    float classify_scale (const points_t& reference_points, const points_t& data_points);

    /**
     * classify the translation from a number of keypoints to the reference
     * Translation (x, y)
     * @param  marker_points [description]
     * @return               [description]
     */
    translation_t classify_translation (const points_t& reference_points,
    const points_t& data_points);

    /**
     * classify the rotation from a number of keypoints to the reference
     * Rotation (yaw)
     * @param  marker_points [description]
     * @return               [description]
     */
    float classify_yaw (const points_t& reference_points, const points_t& data_points);

    // angle pitch
    float classify_pitch (const points_t& reference_points, const points_t& data_points);

    // angle roll
    float classify_roll (const points_t& reference_points, const points_t& data_points);

    /**
     * calculate the centroid of a set of points
     * @param  points are the keypoints of the "shape"
     * @return returns the centroid of the points
     */
    point_t centroid (const points_t& points);

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
    point_t sum (const points_t& points);

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
    point_t intersections (point_t A, point_t B, point_t origin = { 0, 0 });

    /**
     * calculate intersection between two lines
     * @param l1 line
     * @param l2 line
     * @return returns intersection
     */
    point_t intersection (line_t l1, line_t l2);

    /**
     * A of Y=AX+B
     * @param line
     * @return returns A
     */
    float a (line_t line);
    /**
     * B of Y=AX+B
     * @param  line
     * @return returns B
     */
    float b (line_t line);

    /**
     * calculates the x value
     * Y = AX+B
     * @param y the Y parameters
     * @param line
     * @return returns X
     */
    float x (float y, line_t line);

    /**
     * calculates the y value
     * X = (Y-B)/A
     * @param x the X parameters
     * @param line
     * @return returns Y
     */
    float y (float x, line_t line);

    /**
     * calculate absolute distance between two points
     * @param A point A
     * @param B point B
     * @return returns the absolute distance
     */
    float distance (point_t p1, point_t p2);

    /**
     * matches the points to the reference.
     * so that all the points in the marker_points are
     * available in the reference
     * @param marker_points are the reference marker_points
     */
    void match_points (const points_t& reference_points, points_t& data_points);
    /**
     * convert value to an std::string
     * @param value to be converted
     * @return returns a std::string
     */
    template <typename T> std::string to_string (T value);
};

#endif // OPERATORS_HPP
