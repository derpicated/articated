// image.hpp
#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <cstdint>

typedef enum { RGB24 = 0, BGR32, YUV, GREY8, BINARY8 } format_t;

typedef struct image_t {
    uint8_t* data;
    unsigned width;
    unsigned height;
    format_t format;
} image_t;

#endif // IMAGE_HPP
