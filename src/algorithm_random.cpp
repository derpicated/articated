#include "algorithm_random.hpp"
#include "operators.hpp"

#include <cmath>

algorithm_random::algorithm_random ()
: vision_algorithm (0)
, _last_movement ()
, _random_movement () {
    _last_movement.scale (1.0f);
    _last_movement.yaw (1.0f);
    _last_movement.pitch (1.0f);
    _last_movement.roll (1.0f);
}

algorithm_random::~algorithm_random () {
}

void algorithm_random::set_reference () {
    _movement_mutex.lock ();
    translation_t trans = { 0.0f, 0.0f };
    _random_movement.translation (trans);
    _random_movement.scale (0.1f);
    _random_movement.yaw (1.0f);
    _random_movement.pitch (1.0f);
    _random_movement.roll (1.0f);
    _movement_mutex.unlock ();
}

movement3d algorithm_random::execute (const QVideoFrame& const_buffer) {
    image_t image;
    if (frame_to_ram (const_buffer, image)) {
        free (image.data);
    }

    _movement_mutex.lock ();
    movement3d movement = _last_movement + _random_movement;

    if (movement.scale () < 0.2f) {
        _random_movement.scale (std::fabs (_random_movement.scale ()));
    } else if (movement.scale () > 5.0f) {
        _random_movement.scale (-std::fabs (_random_movement.scale ()));
    }

    translation_t trans            = movement.translation ();
    translation_t new_random_trans = _random_movement.translation ();
    if (-1 > trans.x || trans.x > 1) {
        new_random_trans.x = -new_random_trans.x;
    }
    if (-1 > trans.y || trans.y > 1) {
        new_random_trans.y = -new_random_trans.y;
    }
    _random_movement.translation (new_random_trans);

    movement.yaw (std::fmod (movement.yaw (), 360.0f));
    movement.pitch (std::fmod (movement.pitch (), 360.0f));
    movement.roll (std::fmod (movement.roll (), 360.0f));

    _last_movement = movement;
    _movement_mutex.unlock ();

    return movement;
}
