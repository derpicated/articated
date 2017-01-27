#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include <cfloat>
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
     * get scale from a number of keypoints to the reference
     * Scale (z)
     * @param  marker_points [description]
     * @return               [description]
     */
    float scale (const points_t& reference_points,
    const points_t& data_points,
    unsigned int granularity = 5);

    /**
     * classify the translation from a number of keypoints to the reference
     * Translation (x, y)
     * @param  marker_points [description]
     * @return               [description]
     */
    translation_t translation (const points_t& reference_points, const points_t& data_points);

    /**
     * classify the rotation from a number of keypoints to the reference
     * Rotation (yaw)
     * @param  marker_points [description]
     * @return               [description]
     */
    float classify_yaw (const points_t& reference_points, const points_t& data_points);

    // angle pitch
    float pitch (const points_t& reference_points, const points_t& data_points);

    // angle roll
    float roll (const points_t& reference_points, const points_t& data_points);

    /**
     * translate a set of points
     * @param points [description]
     * @param T      [description]
     */
    void translate (points_t& points, translation_t T);

    /**
     * calculates the delta angle between two vectors
     * the shortest angle from p1 to p2
     * where the origin is 0,0 using the dot product method
     * in range of 0 - 2PI
     * @param  p1 [description]
     * @param  p2 [description]
     * @return returns the angle in radians
     */
    float dot_product (const point_t& p1, const point_t& p2);

    /**
     * calculates the delta angle between two vectors
     * the shortest angle from p1 to p2
     * where the origin is 0,0 using the dot product method
     * in range of 0 - 360 degrees
     * @param  p1 [description]
     * @param  p2 [description]
     * @return returns the angle in degrees
     */
    float dot_product_degrees (const point_t& p1, const point_t& p2);

    /**
     * calculates the absolute delta angle
     * in this example 'R' and 'D' will be switched
     * when D>R
     *
     *   R/|
     *   /
     *  /a
     * |---|
     *   D
     * a: alpha (angle)
     * R: first value(reference)
     * D: second value(Data)
     * a (R>D): cos-1(D/R)
     * a (D>R): cos-1(R/D)
     *
     * @param  ref t
     * @param  data [description]
     * @return angle in degrees
     */
    float projected_angle_abs (const float R, const float D);

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
     * find closes number in a vector to a value
     * @param  vec vector with values
     * @param  compare value to compare to
     * @return returns value from vector closes to compare
     */
    float closest (const std::vector<float>& vec, float compare);

    /**
     * check if a point is in front of a point on a line
     * - there is no checking if point is on line
     * - returns false if p1 and p2 from line are the same
     * @param L Line where p1 is the front and p2 is the back
     * @param R the reference from which to check
     * @param P the point that is checked
     * @return returns true if in front, false if not
     */
    bool is_in_front (line_t L, point_t R, point_t P);

    /**
     * check if value is equal to each other using error values
     * @param  a     val1
     * @param  b     val2
     * @param  error the error
     * @return true if equal, false of not equal
     */
    bool equal (float a, float b, float error = FLT_EPSILON);

    /**
     * compare floats using ULP and absolute method
     * https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
     * @param  A           [description]
     * @param  B           [description]
     * @param  maxDiff     [description]
     * @param  maxUlpsDiff [description]
     * @return             [description]
     */
    bool almost_equal_ulp_abs (float A, float B, float maxDiff, int maxUlpsDiff);

    /**
     * compare floats using relative and absolute method
     * https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
     * @param  A          [description]
     * @param  B          [description]
     * @param  maxDiff    [description]
     * @param  maxRelDiff [description]
     * @return            [description]
     */
    bool almost_equal_rel_abs (float A, float B, float maxDiff, float maxRelDiff = FLT_EPSILON);

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
     * find an intersection between two points from "ref" through "line"
     * where the intersection is no
     * - 0 (centroid of ref points)
     * - NaN
     * - x or y is inf (parallel)
     * then the matching intersection is found in the "data" points
     * then the distance from "ref" centroid to "ref" intersection
     * and the distance from "data" centroid to "data" intersection
     * is returned
     *
     * @param reference_points reference points
     * @param data_points data points
     * @param line        [description]
     * @param ref_length  [description]
     * @param data_length [description]
     */
    void find_matching_intersection (const points_t& reference_points,
    const points_t& data_points,
    const line_t& line,
    float& ref_length,
    float& data_length);

    /**
     * convert value to an std::string
     * @param value to be converted
     * @return returns a std::string
     */
    template <typename T> std::string to_string (T value);
};

#endif // OPERATORS_HPP
