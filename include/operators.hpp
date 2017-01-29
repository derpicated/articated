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
 *
 *
 */
typedef enum { RGB24 = 0, GREY8, BINARY8 } format_t;

typedef struct image_t {
    uint8_t* data;
    unsigned width;
    unsigned height;
    format_t format;
} image_t;


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
     * apply filters on image
     * @param   data    pointer to greyscale-image buffer
     * @param   width   width of image (colum count)
     * @param   height  height of image (row count)
     */
    void preprocessing (image_t& image);

    /**
     * segment the greyscale image to binary
     * @param   data    pointer to image buffer
     * @param   width   width of image (colum count)
     * @param   height  height of image (row count)
     */
    void segmentation (image_t& image);


    /**
     * extract the usefull infomration from the binary-image
     * @param   data    pointer to image buffer
     * @param   width   width of image (colum count)
     * @param   height  height of image (row count)
     */
    void extraction (image_t& image);

    /**
     * preform a greyscale "average filter" on image, with window size n*n
     * @param   image   the greyscale image
     * @param   n       the window size, has to be odd
     */
    void filter_average (image_t& image, unsigned n);

    /**
    * calculate the centroid of a blob
    * @param    image   a blob-labeled image
    * @return   number of blobs
    */
    uint32_t label_blobs (image_t& image);

    /**
    * replace all pixes of old value with new value
    * @param    image   a binary image
    * @param    old_value       old value
    * @param    new_value       new value
    */
    void replace_value (image_t& image, uint8_t old_value, uint8_t new_value);


    /**
    * find the lowest value of any of the 8 surronding neighbours of pixel [x,y]
    * @param    image   a binary image
    * @param    x       x coordinate
    * @param    y       y coordinate
    * @return   lowest value in neighbourhood
    */
    uint8_t neighbours_minimum (image_t& image, int x, int y);

    /**
    * calculate the centroid of a blob
    * @param    image   a blob-labeled image
    * @param    blobnr  nr of the blob to be analized
    * @return   centroid point
    */
    point_t centroid (image_t& image, uint8_t blobnr);

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
     * calculate absolute distance between two points
     * @param A point A
     * @param B point B
     * @return returns the absolute distance
     */
    float distance (point_t A, point_t B);
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
