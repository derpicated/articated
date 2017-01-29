#include "movement3d_filter.hpp"
#include "movement3d.hpp"

movement3d_average::movement3d_average (unsigned int samples)
: _samples (samples) {
}

movement3d_average::~movement3d_average () {
}

movement3d movement3d_average::average () {
    movement3d average;
    for (movement3d movement : _movements) {
        average += movement;
    }
    if (!_movements.empty ()) {
        average /= _movements.size ();
    }
    return average;
}

movement3d movement3d_average::average (movement3d movement) {
    _movements.push_back (movement);
    if (_movements.size () > _samples) {
        _movements.pop_front ();
    }
    return average ();
}
