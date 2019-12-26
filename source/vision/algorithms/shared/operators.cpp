#include "operators.hpp"

#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <stdint.h>
#include <string>

#include "movement3d/movement3d.hpp"

bool operator< (const keypoint_t& point, const keypoint_t& other_point) {
    bool ans = false;
    if (point.p.x < other_point.p.x) {
        ans = true;
    } else if (point.p.x == other_point.p.x && point.p.y < other_point.p.y) {
        ans = true;
    }
    return ans;
}
bool operator!= (const keypoint_t& point, const keypoint_t& other_point) {
    bool ans = true;
    if (point.p.x == other_point.p.x && point.p.y == other_point.p.y) {
        ans = false;
    }
    return ans;
}

operators::operators () {
}

operators::~operators () {
}

void operators::preprocessing (image_t& image) {
    if (image.format != GREY8) {
        convert_to_grey (image);
    }
    filter_and_scale (image, 5);
}

void operators::segmentation (image_t& image) {
    threshold_iso_data (image, DARK);
    remove_border_blobs (image, FOUR);
}

void operators::extraction (image_t& image, points_t& markers) {
    unsigned blob_count;
    std::vector<keypoint_t> blobs;
    const unsigned MIN_AREA = 20;

    blob_count = label_blobs (image);
    analyse_blobs (image, blob_count, MIN_AREA, blobs);

    // marker extraction
    std::vector<std::vector<keypoint_t>> potential_markers;
    extract_groups (blobs, potential_markers);
    extract_markers (potential_markers, markers);

    // debug image
    for (auto marker : markers) {
        int x = marker.second.x;
        int y = marker.second.y;

        // set a crosshair at marker position
        image.data[((y - 2) * image.width) + x] = 255;
        image.data[((y - 1) * image.width) + x] = 255;
        image.data[((y + 1) * image.width) + x] = 255;
        image.data[((y + 2) * image.width) + x] = 255;
        image.data[(y * image.width) + x - 2]   = 255;
        image.data[(y * image.width) + x - 1]   = 255;
        image.data[(y * image.width) + x + 1]   = 255;
        image.data[(y * image.width) + x + 2]   = 255;
    }
}

bool operators::classification (const points_t& reference, const points_t& data, Movement3D& movement) {
    Classification classify;
    points_t ref_points = reference;
    points_t points     = data;
    // match points
    classify.match_points (ref_points, points);
    classify.match_points (points, ref_points);
    if (points.size () < classify._minimum_ref_points ||
    points.size () > classify._maximum_ref_points) {
        return false;
    }
    // find translation
    movement.translation (classify.translation (ref_points, points));
    // find scale and apply scale
    movement.scale (classify.scale (ref_points, points));
    classify.scale (points, 1 / movement.scale ());
    // find yaw
    movement.yaw (classify.yaw (ref_points, points));
    // find pitch
    movement.pitch (classify.pitch (ref_points, points));
    // find roll
    movement.roll (classify.roll (ref_points, points));
    return true;
}

void operators::convert_to_grey (image_t& image) {
    unsigned width  = image.width;
    unsigned height = image.height;

    if (image.format == RGB24) {
        // RGB24 to grey transform
        uint8_t* px_past_end  = image.data + (height * width * 3);
        uint8_t* px_RGB_curr  = image.data;
        uint8_t* px_GREY_curr = image.data;
        for (; px_RGB_curr < px_past_end; px_RGB_curr += 3, px_GREY_curr++) {
            *px_GREY_curr = (*px_RGB_curr + *(px_RGB_curr + 1) + *(px_RGB_curr + 2)) / 3;
        }
    }
    if (image.format == BGR32) {
        // BGR32 to grey transform
        uint8_t* px_past_end  = image.data + (height * width * 4);
        uint8_t* px_BGR_curr  = image.data;
        uint8_t* px_GREY_curr = image.data;
        for (; px_BGR_curr < px_past_end; px_BGR_curr += 4, px_GREY_curr++) {
            *px_GREY_curr = (*px_BGR_curr + *(px_BGR_curr + 1) + *(px_BGR_curr + 2)) / 3;
        }
    }

    image.format = GREY8;
}

void operators::filter_and_scale (image_t& image, unsigned n) {
    int height           = image.height;
    int width            = image.width;
    float maskval        = 1.0f / (float)(n * n);
    int half_window_size = n / 2;
    int new_width;
    int new_height;
    int scale_factor;

    // copy buffer data
    uint8_t* image_buffer = new uint8_t[image.width * image.height];
    memcpy (image_buffer, image.data, sizeof (uint8_t) * height * width);

    // set scaling if needed
    if ((width * height) > MAX_PIXEL_COUNT) {
        scale_factor = 4;
        new_width    = width / scale_factor;
        new_height   = height / scale_factor;

        image.width  = new_width;
        image.height = new_height;
    } else {
        scale_factor = 1;
        new_width    = width;
        new_height   = height;
    }

    // for every pixel in src
    for (int y = new_height - 1; y >= 0; --y) {
        for (int x = new_width - 1; x >= 0; --x) {
            // for every pixel in new image
            int original_x   = x * scale_factor;
            int original_y   = y * scale_factor;
            float new_px_val = 0;

            for (int b = -half_window_size; b <= half_window_size; ++b) {
                for (int a = -half_window_size; a <= half_window_size; ++a) {
                    // ensure that normalized window coordinates are
                    // withing src borders, other wise "extend" edges
                    int adjusted_x = original_x + a;
                    int adjusted_y = original_y + b;
                    if (adjusted_x >= width) {
                        adjusted_x = width - 1;
                    } else if (adjusted_x < 0) {
                        adjusted_x = 0;
                    }
                    if (adjusted_y >= height) {
                        adjusted_y = height - 1;
                    } else if (adjusted_y < 0) {
                        adjusted_y = 0;
                    }
                    // calculate new px value
                    new_px_val +=
                    (image_buffer[(adjusted_y * width) + adjusted_x] * maskval);
                }
            }
            // store the new px value in dst
            image.data[(y * (new_width)) + x] = new_px_val;
        }
    }
    delete[] image_buffer;
}

void operators::histogram (image_t& image, uint16_t* hist, uint32_t& sum) {
    int pixels    = image.height * image.width;
    int pixel     = pixels;
    uint8_t* data = (uint8_t*)image.data;
    for (pixel = pixels; pixel != 0; --pixel) {
        *(hist + (*(data + (pixels - pixel)))) += 1;
        sum += *(data + (pixels - pixel));
    }
    return;
}

void operators::threshold (image_t& image, uint8_t low, uint8_t high) {
    int pixels    = image.height * image.width;
    uint8_t* data = (uint8_t*)image.data;
    int pixel     = pixels;

    for (pixel = pixels; pixel != 0; --pixel) {
        if ((*(data + (pixels - pixel)) >= low) && (*(data + (pixels - pixel)) <= high)) {
            *(data + (pixels - pixel)) = 255;
        } else {
            *(data + (pixels - pixel)) = 0;
        }
    }
    image.format = BINARY8;
    return;
}

void operators::threshold_iso_data (image_t& image, eBrightness brightness) {
    const int pValues   = 256;
    uint16_t H[pValues] = { 0 }; // histogram
    uint8_t lPixel      = 255;
    uint8_t hPixel      = 0;
    int T               = 0; // mean between bright and dark
    int Told            = 0; // old mean
    uint32_t sum;
    histogram (image, H, sum);
    int cnt = 0; // some random counter for... counting
    // find hPixel
    for (cnt = pValues; cnt != 0; --cnt) {
        if (H[cnt - 1]) {
            hPixel = cnt - 1;
            cnt    = 1; // not 0 because for loop
        }
    }
    // find lPixel
    for (cnt = pValues; cnt != 0; --cnt) {
        if (H[pValues - cnt]) {
            lPixel = pValues - cnt;
            cnt    = 1; // not 0 because for loop
        }
    }
    // check for zero or same value
    if (lPixel == hPixel) {
        T = lPixel;
    } else {
        T = (int)(lPixel + hPixel) / 2 + 0.5; // center of pixels
        uint32_t meanDark   = 0;              // mean dark (from 0 to T)
        uint32_t meanBright = 0; // mean bright (from T to and including end)
        while (Told != T) {
            Told          = T;
            uint32_t pCnt = 0; // pixels
            // mean left (using Told)
            // 0 to Told
            meanDark = 0;
            pCnt     = 0;
            for (cnt = 0; cnt <= Told; ++cnt) {
                meanDark += cnt * H[cnt]; // pixel value
                pCnt += H[cnt];           // pixel count
            }
            meanDark /= pCnt;

            // mean right (using Told)
            // Told to end
            meanBright = 0;
            pCnt       = 0;
            for (cnt = 255; cnt > Told; --cnt) {
                meanBright += cnt * H[cnt]; // pixel value
                pCnt += H[cnt];             // pixel count
            }
            meanBright /= pCnt;
            // mean of means (rounded)
            T = (int)(meanDark + meanBright) / 2 + 0.5;
        }
    }
    // threshold using T
    if (brightness == DARK) {
        threshold (image, 0, T);
        // std::cout << "Dark: " << T << '\n';
    } else {
        threshold (image, T, 255);
        // std::cout << "Bright: " << T << '\n';
    }
    return;
}

void operators::copy (image_t& src, image_t& dst) {
    int pixels       = src.height * src.width;
    uint8_t* dataSrc = (uint8_t*)src.data;
    uint8_t* dataDst = (uint8_t*)dst.data;
    int pixel        = pixels;

    for (pixel = pixels; pixel != 0; --pixel) {
        *(dataDst + (pixels - pixel)) = *(dataSrc + (pixels - pixel));
    }
    dst.height = src.height;
    dst.width  = src.width;
    dst.format = src.format;
    return;
}

void operators::set_borders (image_t& image, uint8_t value) {
    unsigned int cnt = 0;
    uint8_t* data    = (uint8_t*)image.data;

    for (cnt = 0; cnt != image.width; ++cnt) {
        // top border
        *(data + cnt) = value;
        // bottom border
        *(data + (image.height - 1) * image.width + cnt) = value;
    }

    for (cnt = 0; cnt != image.height; ++cnt) {
        // left border
        *(data + cnt * image.width) = value;
        // right border
        *(data + cnt * image.width + (image.width - 1)) = value;
    }
    return;
}

void operators::set_selected_to_value (image_t& image, uint8_t selected, uint8_t value) {
    int pixels    = image.height * image.width;
    uint8_t* data = (uint8_t*)image.data;
    int pixel     = pixels;

    for (pixel = pixels; pixel != 0; --pixel) {
        if (*(data + (pixels - pixel)) == selected) {
            *(data + (pixels - pixel)) = value;
        }
    }
    return;
}

uint8_t operators::neighbour_count (image_t& img, uint32_t x, uint32_t y, uint8_t value, eConnected connected) {
    uint8_t count = 0;
    uint8_t* data = (uint8_t*)img.data;
    // uint32_t location = y * img.width + x;
    /**
     * pixel directions
     * P = x,y
     * NW N NE
     * W  p  E
     * SW S SE
     */
    switch (connected) {
        case EIGHT:
            // North West
            if ((x - 1 < img.width) && (y - 1 < img.height)) {
                // count += img->data[y - 1][x - 1] == value ? 1 : 0;
                count += *(data + ((y - 1) * img.width) + (x - 1)) == value ? 1 : 0;
            }
            // North East
            if ((x + 1 < img.width) && (y - 1 < img.height)) {
                // count += img->data[y - 1][x + 1] == value ? 1 : 0;
                count += *(data + ((y - 1) * img.width) + (x + 1)) == value ? 1 : 0;
            }
            // South East
            if ((x + 1 < img.width) && (y + 1 < img.height)) {
                // count += img->data[y + 1][x + 1] == value ? 1 : 0;
                count += *(data + ((y + 1) * img.width) + (x + 1)) == value ? 1 : 0;
            }
            // South West
            if ((x - 1 < img.width) && (y + 1 < img.height)) {
                // count += img->data[y + 1][x - 1] == value ? 1 : 0;
                count += *(data + ((y + 1) * img.width) + (x - 1)) == value ? 1 : 0;
            }
        case FOUR:
            // North
            if ((x < img.width) && (y - 1 < img.height)) {
                // count += img->data[y - 1][x] == value ? 1 : 0;
                count += *(data + ((y - 1) * img.width) + (x)) == value ? 1 : 0;
            }
            // East
            if ((x + 1 < img.width) && (y < img.height)) {
                // count += img->data[y][x + 1] == value ? 1 : 0;
                count += *(data + ((y)*img.width) + (x + 1)) == value ? 1 : 0;
            }
            // South
            if ((x < img.width) && (y + 1 < img.height)) {
                // count += img->data[y + 1][x] == value ? 1 : 0;
                count += *(data + ((y + 1) * img.width) + (x)) == value ? 1 : 0;
            }
            // West
            if ((x - 1 < img.width) && (y < img.height)) {
                // count += img->data[y][x - 1] == value ? 1 : 0;
                count += *(data + ((y)*img.width) + (x - 1)) == value ? 1 : 0;
            }
    }
    return count;
}

void operators::remove_border_blobs (image_t& img, eConnected connected) {
    uint32_t pixels         = img.width * img.height;
    uint8_t* data           = (uint8_t*)img.data;
    unsigned int changed    = false;
    unsigned int forward    = true;
    unsigned int iterations = 1;
    set_borders (img, 2); // set borders to value
    do {
        changed    = false;
        uint32_t i = 0;
        for (i = 0; i < pixels; i++) {
            const uint32_t pixel = forward ? i : (pixels - 1) - i;
            const uint32_t row   = pixel / img.width;
            const uint32_t coll  = pixel - row * img.width;
            if (*(data + i) == 255) {
                if (neighbour_count (img, coll, row, 2, connected)) {
                    *(data + i) = 2;
                    changed     = true;
                }
            }
        }
        iterations++;
        forward = !forward;
    } while (changed);
    iterations++;
    // std::cout << "iterations: " << iterations << std::endl;
    set_selected_to_value (img, 2, 0);
    return;
}

uint32_t operators::label_blobs (image_t& image) {
    /*
    a pixel with the value 255 is considered a marker, UIDs range from 1 to
    254
    */
    int height   = image.height;
    int width    = image.width;
    unsigned UID = 1;

    bool changed;
    do {
        changed = false;
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

void operators::analyse_blobs (image_t& img,
const unsigned blob_count,
const unsigned min_area,
std::vector<keypoint_t>& blobs) {
    int blob_info[blob_count][3]; // px_count, sum_x, sum_y
    int x;
    int y;
    int width  = img.width;
    int height = img.height;
    uint8_t i;

    memset (blob_info, 0, sizeof (int) * blob_count * 3);
    // for each pixel in image
    for (y = height - 1; y >= 0; --y) {
        for (x = width - 1; x >= 0; --x) {
            uint8_t value = img.data[(y * width) + x];
            if (0 < value && value <= blob_count) {
                --value;                  // to be used as index
                blob_info[value][0]++;    // px_count
                blob_info[value][1] += x; // sum_x
                blob_info[value][2] += y; // sum_y
            }
        }
    }

    for (i = 0; i < blob_count; ++i) {
        unsigned px_count = blob_info[i][0];
        unsigned sum_x    = blob_info[i][1];
        unsigned sum_y    = blob_info[i][2];
        if (px_count > min_area) {
            keypoint_t blob;
            blob.p.x = (sum_x / (float)(px_count)) + 0.5f;
            blob.p.y = (sum_y / (float)(px_count)) + 0.5f;
            blob.id  = sqrt (px_count / (M_PI * 2));
            blobs.push_back (blob);
        }
    }
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

void operators::extract_groups (std::vector<keypoint_t> key_points,
std::vector<std::vector<keypoint_t>>& potential_markers) {
    // group keypoints into markers by proximity
    const int BLOB_SIZE_RATIO = 10;
    std::map<keypoint_t, std::vector<keypoint_t>> neighbours;

    // get all neighbours for each blob
    for (keypoint_t point : key_points) {
        float range = point.id * BLOB_SIZE_RATIO;
        float x     = point.p.x;
        float y     = point.p.y;

        for (keypoint_t other_point : key_points) {
            if (point != other_point) {
                float dx       = x - other_point.p.x;
                float dy       = y - other_point.p.y;
                float distance = sqrt ((dx * dx) + (dy * dy));

                // if other_point is in range, group it with point
                if (distance < range) {
                    neighbours[point].push_back (other_point);
                }
            }
        }
    }

    // recursively link all neighbours into groups
    while (!neighbours.empty ()) {
        std::vector<keypoint_t> potential_marker;
        extract_groups_link (neighbours, potential_marker, neighbours.begin ()->first);
        potential_markers.push_back (potential_marker);
    }
}

void operators::extract_groups_link (std::map<keypoint_t, std::vector<keypoint_t>>& neighbours,
std::vector<keypoint_t>& potential_marker,
const keypoint_t& point) {
    // if the point hasnt been processed yet
    if (neighbours.find (point) != neighbours.end ()) {
        // add the point to the markert
        // get its neighbours
        // and remove the from the unprocessed lis
        potential_marker.push_back (point);
        std::vector<keypoint_t> neighbour_list = neighbours[point];
        neighbours.erase (point);

        for (keypoint_t neighbour_point : neighbour_list) {
            // link all the neighbours neighbours
            extract_groups_link (neighbours, potential_marker, neighbour_point);
        }
    }
}

void operators::extract_markers (std::vector<std::vector<keypoint_t>>& potential_markers,
points_t& markers) {
    const unsigned int MIN_MARKER_ID = 2;
    const unsigned int MAX_MARKER_ID = 9;
    // calculate marker properties (id, size, location)
    for (std::vector<keypoint_t> marker_points : potential_markers) {
        const unsigned int marker_id = marker_points.size ();

        if (MIN_MARKER_ID <= marker_id && marker_id <= MAX_MARKER_ID) {
            point_t marker_pos;
            float average_x = 0;
            float average_y = 0;

            for (keypoint_t key_point : marker_points) {
                //    average_size += key_point.size;
                average_x += key_point.p.x;
                average_y += key_point.p.y;
            }

            // average_size /= marker_id;
            marker_pos.x       = average_x / marker_id;
            marker_pos.y       = average_y / marker_id;
            markers[marker_id] = marker_pos;
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
