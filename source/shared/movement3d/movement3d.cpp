#include "movement3d.hpp"
#include <iostream>

Movement3D::Movement3D ()
: _scale (0)
, _translation ({ 0, 0 })
, _yaw (0)
, _pitch (0)
, _roll (0) {
}
Movement3D::~Movement3D () {
}

float Movement3D::translation_delta_to_absolute (const float d_value,
const int ref_width,
const float t_min,
const float t_max) const {
    return (((t_max - t_min) * d_value) / ref_width);
}

void Movement3D::scale (const float s) {
    _scale = s;
}
float Movement3D::scale () const {
    return _scale;
}

void Movement3D::translation (const translation_t& t) {
    _translation = t;
}
translation_t Movement3D::translation () const {
    return _translation;
}

void Movement3D::yaw (const float y) {
    _yaw = y;
}
float Movement3D::yaw () const {
    return _yaw;
}

void Movement3D::pitch (const float p) {
    _pitch = p;
}
float Movement3D::pitch () const {
    return _pitch;
}

void Movement3D::roll (const float r) {
    _roll = r;
}
float Movement3D::roll () const {
    return _roll;
}

std::ostream& operator<< (std::ostream& os, const Movement3D& movement) {
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

Movement3D& Movement3D::operator+= (const Movement3D& movement) {
    this->scale (this->scale () + movement.scale ());

    this->translation ({ this->translation ().x + movement.translation ().x,
    this->translation ().y + movement.translation ().y });

    this->yaw (this->yaw () + movement.yaw ());
    this->pitch (this->pitch () + movement.pitch ());
    this->roll (this->roll () + movement.roll ());

    return *this;
}

Movement3D Movement3D::operator+ (const Movement3D& movement) {
    *this += movement;
    return *this;
}

Movement3D& Movement3D::operator/= (const float factor) {
    this->scale (this->scale () / factor);

    this->translation ({ this->translation ().x / factor, this->translation ().y / factor });

    this->yaw (this->yaw () / factor);
    this->pitch (this->pitch () / factor);
    this->roll (this->roll () / factor);

    return *this;
}

Movement3D Movement3D::operator/ (const float factor) {
    *this /= factor;
    return *this;
}
