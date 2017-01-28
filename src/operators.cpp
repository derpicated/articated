#include "operators.hpp"
#include <cmath>
#include <iostream>
#include <map>
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

void operators::preprocessing (image_t& image) {
    unsigned width  = image.width;
    unsigned height = image.height;

    // RGB to grey transform
    uint8_t* px_past_end  = image.data + (height * width * 3);
    uint8_t* px_RGBA_curr = image.data;
    uint8_t* px_GREY_curr = image.data;
    for (; px_RGBA_curr < px_past_end; px_RGBA_curr += 3, px_GREY_curr++) {
        *px_GREY_curr = (*px_RGBA_curr + *(px_RGBA_curr + 1) + *(px_RGBA_curr + 2)) / 3;
        //*px_GREY_curr = 100;
    }
    image.format = GREY8;
    // image.width  = width;
    // image.height = height * 4;
}

void operators::segmentation (image_t& image) {
    int height = image.height;
    int width  = image.width;
    unsigned hist[256];
    uint32_t sum;
    unsigned bcv;
    int i;
    int j;
    unsigned max_bcv       = 0;
    unsigned max_bcv_index = 0;
    unsigned low_mean, high_mean, low_count, high_count;
    uint8_t* px_start = (uint8_t*)image.data;
    uint8_t* px_curr  = px_start + (height * width) - 1;

    // create histogram
    for (; px_curr >= px_start; px_curr--) {
        hist[*px_curr]++;
    }
    // Calculate the total number of pixels;
    // Calculate the sum of all pixels;

    for (i = 255; i >= 0; i--) {
        low_mean   = 0;
        high_mean  = 0;
        low_count  = 0;
        high_count = 0;

        for (j = i - 1; j >= 0; j--) {
            // determine the number of pixels in the object;
            // calculate the sum of the pixels in the object;
            // calculate the meanvalue of the pixels in the object;
            low_mean += hist[j] * j;
            low_count += hist[j];
        }
        for (j = i; j < 256; j++) {
            // determine the number of pixels in the back;
            // calculate the sum of the pixels in the back;
            // calculate the meanvalue of the pixels in the back;
            high_mean += hist[j] * j;
            high_count += hist[j];
        }

        if (low_count > 0 && high_count > 0) {
            low_mean /= low_count;
            high_mean /= high_count;
            bcv = (low_count * high_count) * (low_mean - high_mean) * (low_mean - high_mean);
        } else {
            bcv = 0;
        }

        if (max_bcv < bcv) {
            max_bcv       = bcv;
            max_bcv_index = i;
        }
    }

    // threshold

    for (px_curr = px_start + (height * width) - 1; px_curr >= px_start; px_curr--) {
        if (*px_curr < max_bcv_index) {
            *px_curr = 255;
        } else {
            *px_curr = 0;
        }
    }
}

void operators::extraction (image_t& image) {
    ;
}

uint32_t operators::label_blobs (image_t& image) {
    /*
    a pixel with the value 255 is considered a marker, UIDs range from 1 to
    254
    */
    int height   = image.height;
    int width    = image.width;
    unsigned UID = 1;

    // mark all blob pixels
    uint8_t* px_start = (uint8_t*)image.data;
    uint8_t* px_curr  = px_start + (height * width) - 1;
    for (; px_curr >= px_start; px_curr--) {
        if (*px_curr == 1) {
            *px_curr = 255;
        }
    }

    bool changed = false;
    do {
        // top left to bottom right
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                uint8_t curr_px_val = image.data[(y * width) + x];
                if (curr_px_val != 0) {
                    uint8_t min_px_neigborhood_val = neighbours_minimum (image, x, y);
                    if (min_px_neigborhood_val == 255) {
                        image.data[(y * width) + x] = UID++;
                        changed                     = true;
                    } else if (min_px_neigborhood_val < curr_px_val) {
                        image.data[(y * width) + x] = min_px_neigborhood_val;
                        changed                     = true;
                    }
                }
            }
        }
        // bottom right to top left
        for (int y = height - 1; y >= 0; --y) {
            for (int x = width - 1; x >= 0; --x) {
                uint8_t curr_px_val = image.data[(y * width) + x];
                if (curr_px_val != 0) {
                    uint8_t min_px_neigborhood_val = neighbours_minimum (image, x, y);
                    if (min_px_neigborhood_val == 255) {
                        image.data[(y * width) + x] = UID++;
                        changed                     = true;
                    } else if (min_px_neigborhood_val < curr_px_val) {
                        image.data[(y * width) + x] = min_px_neigborhood_val;
                        changed                     = true;
                    }
                }
            }
        }
    } while (changed);

    UID = 1;
    // make the blob labels sequential
    // has to be TL => BR
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t curr_px_val = image.data[(y * width) + x];
            if (UID < curr_px_val) {
                replace_value (image, curr_px_val, UID);
                UID++;
            } else if (curr_px_val == UID) {
                UID++;
            }
        }
    }

    return UID - 1;
}

void operators::replace_value (image_t& image, uint8_t old_value, uint8_t new_value) {
    unsigned width    = image.width;
    unsigned height   = image.height;
    uint8_t* px_start = (uint8_t*)image.data;
    uint8_t* px_curr  = px_start + (height * width) - 1;
    for (; px_curr >= px_start; px_curr--) {
        if (*px_curr == old_value) {
            *px_curr = new_value;
        }
    }
}

uint8_t operators::neighbours_minimum (image_t& image, int x, int y) {
    unsigned width = image.width;
    // unsigned height       = image.height;
    uint8_t minimum_value = image.data[(y * width) + x];
    uint8_t current_value = image.data[(y * width) + x];

    current_value = image.data[(y * width) + x - 1];
    if (0 < current_value && current_value < minimum_value) {
        minimum_value = current_value;
    }
    current_value = image.data[(y * width) + x + 1];
    if (0 < current_value && current_value < minimum_value) {
        minimum_value = current_value;
    }
    current_value = image.data[((y - 1) * width) + x];
    if (0 < current_value && current_value < minimum_value) {
        minimum_value = current_value;
    }
    current_value = image.data[((y + 1) * width) + x];
    if (0 < current_value && current_value < minimum_value) {
        minimum_value = current_value;
    }
    current_value = image.data[((y - 1) * width) + x - 1];
    if (0 < current_value && current_value < minimum_value) {
        minimum_value = current_value;
    }
    current_value = image.data[((y - 1) * width) + x + 1];
    if (0 < current_value && current_value < minimum_value) {
        minimum_value = current_value;
    }
    current_value = image.data[((y + 1) * width) + x - 1];
    if (0 < current_value && current_value < minimum_value) {
        minimum_value = current_value;
    }
    current_value = image.data[((y + 1) * width) + x + 1];
    if (0 < current_value && current_value < minimum_value) {
        minimum_value = current_value;
    }

    return minimum_value;
}

point_t operators::centroid (image_t& image, uint8_t blobnr) {
    point_t ret;
    int width           = image.width;
    int height          = image.height;
    unsigned sum_x      = 0;
    unsigned sum_y      = 0;
    unsigned nof_pixels = 0;
    for (int y = height - 1; y >= 0; y++) {
        for (int x = width - 1; x >= width; x++) {
            if (image.data[(y * width) + x] == blobnr) {
                nof_pixels++;
                sum_x += x;
                sum_y += y;
            }
        }
    }

    ret.x = sum_x / (float)(nof_pixels) + 0.5f;
    ret.y = sum_y / (float)(nof_pixels) + 0.5f;
    return ret;
}

float operators::classify_scale (const points_t& reference_points, const points_t& data_points) {
    points_t points = data_points;
    float scale     = 1;
    match_points (reference_points, points);
    if (points.size () < _minimum_ref_points || points.size () > _maximum_ref_points) {
        return scale;
    }
    /* centroid */
    point_t centroid_r = centroid (reference_points);
    point_t centroid_d = centroid (points);
    /**
     * find points that go through x, and y axis in reference
     * p_s = start point
     * p_x = point that makes a line through x axis to p_start
     * p_y = point that makes a line through y axis to p_start
     * [p is point, r is reference, _? = variable]
     */
    keypoint_t pr_s = { reference_points.begin ()->first,
        reference_points.begin ()->second };
    keypoint_t pr_x;
    keypoint_t pr_y;
    /**
     * find the ratio between the delta x and delta y
     * from point pr_start to all points
     *        ratio of | means
     *             1.0 | right in between
     * POINT_INF (inf) | horizontal line
     *  POINT_ZERO (0) | vertical line
     */
    std::map<unsigned int, float> point_ratios; // <point ID, ratio>
    const float h_ratio = POINT_INF;
    const float v_ratio = POINT_ZERO;
    point_t delta       = {};
    for (auto point : reference_points) {
        if (pr_s.id != point.first) {
            delta.x = (point.second.x - pr_s.p.x);
            delta.y = (point.second.y - pr_s.p.y);
            if (delta.y == POINT_ZERO) {
                // horizontal line
                // diff y is 0
                point_ratios.insert ({ point.first, h_ratio });
            } else if (delta.x == POINT_ZERO) {
                // vertical line
                // diff x 0
                point_ratios.insert ({ point.first, v_ratio });
            } else {
                // absolute value, 1.0 is right in between
                point_ratios.insert ({ point.first, std::fabs (delta.x / delta.y) });
            }
        }
    }
    /**
     * find optimal value for a horizontal line (will go through the Y-axis)
     * and one for a vertical line (will go through the X-axis)
     */
    /* keys for reference points */
    unsigned int h_ratio_optimal_p = point_ratios.begin ()->first;
    unsigned int v_ratio_optimal_p = point_ratios.begin ()->first;
    for (auto ratio : point_ratios) {
        h_ratio_optimal_p = std::fabs (ratio.second - h_ratio) <
        std::fabs (point_ratios.find (h_ratio_optimal_p)->second - h_ratio) ?
        ratio.first :
        h_ratio_optimal_p;
        v_ratio_optimal_p = std::fabs (ratio.second - v_ratio) <
        std::fabs (point_ratios.find (v_ratio_optimal_p)->second - v_ratio) ?
        ratio.first :
        v_ratio_optimal_p;
    }
    pr_x = { v_ratio_optimal_p, reference_points.find (v_ratio_optimal_p)->second };
    pr_y = { h_ratio_optimal_p, reference_points.find (h_ratio_optimal_p)->second };

    /* intersections through x and y axis on reference */
    point_t pr_sx_intersection = intersections (pr_s.p, pr_x.p, centroid_r);
    pr_sx_intersection.y       = 0;
    // pr_sx_intersection         = { pr_sx_intersection.x - centroid_r.x,
    //     pr_sx_intersection.y - centroid_r.y };
    point_t pr_sy_intersection = intersections (pr_s.p, pr_y.p, centroid_r);
    pr_sy_intersection.x       = 0;
    // pr_sy_intersection         = { pr_sy_intersection.x - centroid_r.x,
    //     pr_sy_intersection.y - centroid_r.y };

    // - for pr_s tot pr_x
    // - for pr_s to pr_y
    // find ratio on where the intersection is
    // ps_s <-------> ps_x
    // ps_s <---> intersection
    //
    // find the tree points in the data
    // using this scale find the intersection in the data
    //
    // calculate reference and data origin to intersection length
    // calculate scale from ref to data
    // for pr_s to pr_y
    // smallest scale is the scale?

    /**
     * find scale
     * ps_s <-------> ps_x
     * ps_s <---> intersection
     */
    float dist_pr_sx  = distance (pr_s.p, pr_x.p);
    float dist_pr_sxi = distance (
    { pr_s.p.x - centroid_r.x, pr_s.p.y - centroid_r.y }, pr_sx_intersection);
    float scale_pr_sx = dist_pr_sxi / dist_pr_sx;
    float dist_pr_sy  = distance (pr_s.p, pr_y.p);
    float dist_pr_syi = distance (
    { pr_s.p.x - centroid_r.x, pr_s.p.y - centroid_r.y }, pr_sy_intersection);
    float scale_pr_sy = dist_pr_syi / dist_pr_sy;

    /**
     * find intersections in data
     */
    keypoint_t pd_s = { data_points.find (pr_s.id)->first,
        data_points.find (pr_s.id)->second };
    keypoint_t pd_x = { data_points.find (pr_x.id)->first,
        data_points.find (pr_x.id)->second };
    keypoint_t pd_y = { data_points.find (pr_y.id)->first,
        data_points.find (pr_y.id)->second };

    point_t pd_sx_intersection = { pd_s.p.x + ((pd_x.p.x - pd_s.p.x) * scale_pr_sx),
        pd_s.p.y + ((pd_x.p.y - pd_s.p.y) * scale_pr_sx) };
    pd_sx_intersection = { pd_sx_intersection.x - centroid_d.x,
        pd_sx_intersection.y - centroid_d.y };

    point_t pd_sy_intersection = { pd_s.p.x + ((pd_y.p.x - pd_s.p.x) * scale_pr_sy),
        pd_s.p.y + ((pd_y.p.y - pd_s.p.y) * scale_pr_sy) };
    pd_sy_intersection = { pd_sy_intersection.x - centroid_d.x,
        pd_sy_intersection.y - centroid_d.y };

    // ratio of intersection
    // <------->
    // <--->
    // A   X   B
    // X
    // calculate vector
    // Y


    // keypoint to use for x and y intersections on data
    // keypoint_t data_x = { h_ratio_optimal, data_points.find
    // (h_ratio_optimal)->second };
    // keypoint_t data_y = { v_ratio_optimal, data_points.find
    // (v_ratio_optimal)->second };

    // [from reference]
    // find line(set of 2 points) that crosses x and y axis at some point
    // make sure that this line is as close to 45 degrees to the centroid
    // so that an overflow isn't bound to happen due to very large values
    //
    // with these points, solve Y=AX+B
    // (intersection points with X and Y axis)
    // where the centroid is the origin
    //
    // note the ratio where the intersection lays
    // on the line between the two points
    //
    // [from data]
    // using the two points and the ratio, look for the x/y intersection
    // and compare the vectors from origin to intersections
    // The one wit the difference can be used to calculate the scale
    // (new size/old size)
    //
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

translation_t operators::classify_translation (const points_t& reference_points,
const points_t& data_points) {
    translation_t translation = { 0, 0 };
    points_t points           = data_points;
    match_points (reference_points, points);
    if (points.size () <= _minimum_ref_points) {
        return translation;
    }
    point_t centroid_reference = centroid (reference_points);
    point_t centroid_points    = centroid (points);
    translation.x              = centroid_points.x - centroid_reference.x;
    translation.y              = centroid_points.y - centroid_reference.y;
    return translation;
}

float operators::classify_yaw (const points_t& reference_points, const points_t& data_points) {
    return 0;
}

float operators::classify_pitch (const points_t& reference_points, const points_t& data_points) {
    return 0;
}

float operators::classify_roll (const points_t& reference_points, const points_t& data_points) {
    return 0;
}

point_t operators::centroid (const points_t& points) {
    point_t centroid = { 0, 0 };
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

point_t operators::sum (const points_t& points) {
    point_t keypoint_sum     = {};
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

point_t operators::intersections (point_t p1, point_t p2, point_t origin) {
    point_t I = { 0, 0 }; // intersection
    // normalize using the origin
    p1 = { p1.x - origin.x, p1.y - origin.y };
    p2 = { p2.x - origin.x, p2.y - origin.y };
    // Y = AX + B
    float dY = p2.y - p1.y;
    float dX = p2.x - p1.x;
    // A
    float A;
    // horizontal line,
    // dY == 0 and p1.y == 0, x = 0
    // else x = inf
    if (dY == POINT_ZERO) {
        I.y = p1.y;
        if (p1.y == POINT_ZERO) {
            I.x = POINT_ZERO;
        } else {
            I.x = POINT_INF;
        }
        return I;
    }
    // vertical line
    // dX == 0 and pi.x == 0, y = 0
    // else Y = inf
    if (dX == POINT_ZERO) {
        I.x = p1.x;
        if (p1.x == POINT_ZERO) {
            I.y = POINT_ZERO;
        } else {
            I.y = POINT_INF;
        }
        return I;
    }
    A = dY / dX;
    // B
    float B = p1.y - A * p1.x;
    // intersection X axis; Y = 0
    // x = (y - B) / A
    I.x = (0 - B) / A;
    // intersection Y axis; X = 0
    // y = Ax+b = b
    I.y = B;
    I   = { I.x, I.y };
    return I;
}

float operators::distance (point_t A, point_t B) {
    point_t d = { std::fabs (B.x - A.x), std::fabs (B.y - A.y) };
    return std::sqrt (d.x * d.x + d.y * d.y);
}

void operators::match_points (const points_t& reference_points, points_t& data_points) {
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
