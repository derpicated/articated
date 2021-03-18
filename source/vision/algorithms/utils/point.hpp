// point.hpp
#ifndef POINT_HPP
#define POINT_HPP

#include <cfloat>
#include <limits>
#include <map>
#include <numeric>

const float POINT_MIN  = std::numeric_limits<float>::min ();
const float POINT_ZERO = 0;
const float POINT_MAX  = std::numeric_limits<float>::max ();
const float POINT_INF  = std::numeric_limits<float>::infinity ();

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

typedef std::map<unsigned int, point_t> points_t;

#endif // POINT_HPP
