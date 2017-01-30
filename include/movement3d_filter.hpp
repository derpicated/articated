#ifndef movement3d_filter_HPP
#define movement3d_filter_HPP

#include "movement3d.hpp"
#include <GL/gl.h>
#include <deque>
#include <iostream>

class movement3d_average {
    std::deque<movement3d> _movements;
    unsigned int _samples = 1;

    private:
    public:
    movement3d_average (unsigned int samples = 1);
    ~movement3d_average ();

    /**
     * calculate average between all movements stored
     * @return           returns the calculated movement
     */
    movement3d average ();

    /**
     * add a movement and calculate average between all movements stored
     * @param  movement a movement to add
     * @return          returns the calculated movement
     */
    movement3d average (movement3d movement);
};

#endif // movement3d_filter_HPP
