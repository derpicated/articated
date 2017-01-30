#include "movement3d.hpp"
#include "operators.hpp"
#include <iostream>

movement3d::movement3d ()
: _scale (0)
, _translation ({ 0, 0 })
, _yaw (0)
, _pitch (0)
, _roll (0) {
}
movement3d::~movement3d () {
}

float movement3d::translation_delta_to_absolute (const float d_value,
const int ref_width,
const float t_min,
const float t_max) const {
    return (((t_max - t_min) * d_value) / ref_width);
}

void movement3d::scale (const float s) {
    _scale = s;
}
float movement3d::scale () const {
    return _scale;
}

void movement3d::translation (const translation_t& t) {
    _translation = t;
}
translation_t movement3d::translation () const {
    return _translation;
}

void movement3d::yaw (const float y) {
    _yaw = y;
}
float movement3d::yaw () const {
    return _yaw;
}

void movement3d::pitch (const float p) {
    _pitch = p;
}
float movement3d::pitch () const {
    return _pitch;
}

void movement3d::roll (const float r) {
    _roll = r;
}
float movement3d::roll () const {
    return _roll;
}

std::ostream& operator<< (std::ostream& os, const movement3d& movement) {
    (void)os;
    (void)movement;
    os << "T(" << movement.translation ().x << "," << movement.translation ().y
       << ")" << std::endl;
    os << "S: " << movement.scale () << std::endl;
    os << "yaw: " << movement.yaw () << std::endl;
    os << "pitch: " << movement.pitch () << std::endl;
    os << "roll: " << movement.roll () << std::endl;
    return os;
}

movement3d& movement3d::operator+= (const movement3d& movement) {
    this->scale (this->scale () + movement.scale ());

    this->translation ({ this->translation ().x + movement.translation ().x,
    this->translation ().y + movement.translation ().y });

    this->yaw (this->yaw () + movement.yaw ());
    this->pitch (this->pitch () + movement.pitch ());
    this->roll (this->roll () + movement.roll ());

    return *this;
}

movement3d movement3d::operator+ (const movement3d& movement) {
    *this += movement;
    return *this;
}

movement3d& movement3d::operator/= (const float factor) {
    this->scale (this->scale () / factor);

    this->translation ({ this->translation ().x / factor, this->translation ().y / factor });

    this->yaw (this->yaw () / factor);
    this->pitch (this->pitch () / factor);
    this->roll (this->roll () / factor);

    return *this;
}

movement3d movement3d::operator/ (const float factor) {
    *this /= factor;
    return *this;
}
