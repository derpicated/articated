#ifndef movement3d_HPP
#define movement3d_HPP

#include "operators.hpp"
#include <iostream>

class movement3d {
    private:
    // scale
    float _scale;
    // translation
    translation_t _translation;
    // yaw
    float _yaw;
    // pitch
    float _pitch;
    // roll
    float _roll;

    public:
    movement3d ();
    ~movement3d ();

    movement3d& operator+= (const movement3d& movement);
    movement3d operator+ (const movement3d& movement);

    movement3d& operator/= (const float factor);
    movement3d operator/ (float factor);

    /**
     * convert a translation delta to an absolute value on a plane
     * (t_max - t_min) = steps that can be taken
     * [cross-multiplication]
     * absolute_value   =   steps
     * delta_value      =   ref_width
     * @param  d_value   delta value
     * @param  ref_width reference width (e.g. image width)
     * @param  t_min     translate minimum
     * @param  t_max     translate maximum
     * @return           the absolute value
     */
    float translation_delta_to_absolute (const float d_value,
    const int ref_width,
    const float t_min = -1.0,
    const float t_max = 1.0) const;

    /**
     * rotation in x direction
     * @param x rotation matrix
     */
    void scale (const float s);
    float scale () const;

    void translation (const translation_t& t);
    translation_t translation () const;

    void yaw (const float y);
    float yaw () const;

    void pitch (const float p);
    float pitch () const;

    void roll (const float r);
    float roll () const;
};

// operator functions
std::ostream& operator<< (std::ostream& os, const movement3d& movement);

#endif // movement3d_HPP
