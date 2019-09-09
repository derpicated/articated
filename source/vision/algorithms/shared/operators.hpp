#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include <cfloat>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <vector>

#include "classification.hpp"
#include "image.hpp"
#include "movement3d/movement3d.hpp"
#include "point.hpp"

typedef enum { BRIGHT = 0, DARK } eBrightness;

typedef enum { FOUR = 4, EIGHT = 8 } eConnected;

const int PREFERED_IMAGE_HEIGHT = 480;
const int PREFERED_IMAGE_WIDTH  = 850;
const int MAX_PIXEL_COUNT       = 500000;

class operators {
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
    void extraction (image_t& image, points_t& markers);

    bool classification (const points_t& reference, const points_t& data, Movement3D& movement);

    void convert_to_grey (image_t& image);

    /**
     * preform a greyscale "average filter" on image, with window size n*n
     * @param   image   the greyscale image
     * @param   n       the window size, has to be odd
     */
    void filter_and_scale (image_t& image, unsigned n);

    void histogram (image_t& image, uint16_t* hist, uint32_t& sum);

    void threshold (image_t& image, uint8_t low, uint8_t high);

    void threshold_iso_data (image_t& image, eBrightness brightness);

    void copy (image_t& src, image_t& dst);

    void set_borders (image_t& image, uint8_t value);

    void set_selected_to_value (image_t& image, uint8_t selected, uint8_t value);

    uint8_t neighbour_count (image_t& img, uint32_t x, uint32_t y, uint8_t value, eConnected connected);

    void remove_border_blobs (image_t& img, eConnected connected);

    uint32_t label_blobs (image_t& image);

    void analyse_blobs (image_t& img,
    const unsigned blob_count,
    const unsigned min_area,
    std::vector<keypoint_t>& blobs);

    void extract_groups (std::vector<keypoint_t> key_points,
    std::vector<std::vector<keypoint_t>>& potential_markers);

    void extract_groups_link (std::map<keypoint_t, std::vector<keypoint_t>>& neighbours,
    std::vector<keypoint_t>& potential_marker,
    const keypoint_t& point);

    void extract_markers (std::vector<std::vector<keypoint_t>>& potential_markers,
    points_t& markers);

    /**
     * replace all pixes of old value with new value
     * @param    image   a binary image
     * @param    old_value       old value
     * @param    new_value       new value
     */
    void replace_value (image_t& image, uint8_t old_value, uint8_t new_value);


    /**
     * find the lowest value of any of the 8 surronding neighbours of pixel
     * [x,y]
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
};

#endif // OPERATORS_HPP
