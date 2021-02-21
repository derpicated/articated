#ifndef movement3d_filter_HPP
#define movement3d_filter_HPP

#include "movement3d.hpp"
#include <deque>
#include <iostream>

class Movement3DFilter {
    std::deque<Movement3D> _movements;
    unsigned int _samples = 1;

    private:
    public:
    explicit Movement3DFilter (unsigned int samples = 1);
    ~Movement3DFilter ();

    /**
     * calculate average between all movements stored
     * @return           returns the calculated movement
     */
    Movement3D average ();

    /**
     * add a movement and calculate average between all movements stored
     * @param  movement a movement to add
     * @return          returns the calculated movement
     */
    Movement3D average (Movement3D movement);
};

#endif // movement3d_filter_HPP
