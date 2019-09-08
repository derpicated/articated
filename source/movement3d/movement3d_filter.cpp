#include "movement3d_filter.hpp"
#include "movement3d.hpp"

Movement3DFilter::Movement3DFilter (unsigned int samples)
: _samples (samples) {
}

Movement3DFilter::~Movement3DFilter () {
}

Movement3D Movement3DFilter::average () {
    Movement3D average;
    for (Movement3D movement : _movements) {
        average += movement;
    }
    if (!_movements.empty ()) {
        average /= _movements.size ();
    }
    return average;
}

Movement3D Movement3DFilter::average (Movement3D movement) {
    _movements.push_back (movement);
    if (_movements.size () > _samples) {
        _movements.pop_front ();
    }
    return average ();
}
