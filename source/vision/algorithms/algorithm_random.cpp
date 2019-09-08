#include "algorithm_random.hpp"

#include <cmath>

#include "operators/operators.hpp"


AlgorithmRandom::AlgorithmRandom (QOpenGLContext& opengl_context, augmentation_widget& augmentation)
: VisionAlgorithm (0, opengl_context, augmentation)
, last_movement_ ()
, random_movement_ () {
    last_movement_.scale (1.0f);
    last_movement_.yaw (1.0f);
    last_movement_.pitch (1.0f);
    last_movement_.roll (1.0f);
}

AlgorithmRandom::~AlgorithmRandom () {
}

void AlgorithmRandom::SetReference () {
    movement_mutex_.lock ();
    translation_t trans = { 0.0f, 0.0f };
    random_movement_.translation (trans);
    random_movement_.scale (0.1f);
    random_movement_.yaw (1.0f);
    random_movement_.pitch (1.0f);
    random_movement_.roll (1.0f);
    movement_mutex_.unlock ();
}

movement3d AlgorithmRandom::Execute (const QVideoFrame& const_buffer) {
    image_t image;
    if (FrameToRam (const_buffer, image)) {
        free (image.data);
    }

    movement_mutex_.lock ();
    movement3d movement = last_movement_ + random_movement_;

    if (movement.scale () < 0.2f) {
        random_movement_.scale (std::fabs (random_movement_.scale ()));
    } else if (movement.scale () > 5.0f) {
        random_movement_.scale (-std::fabs (random_movement_.scale ()));
    }

    translation_t trans            = movement.translation ();
    translation_t new_random_trans = random_movement_.translation ();
    if (-1 > trans.x || trans.x > 1) {
        new_random_trans.x = -new_random_trans.x;
    }
    if (-1 > trans.y || trans.y > 1) {
        new_random_trans.y = -new_random_trans.y;
    }
    random_movement_.translation (new_random_trans);

    movement.yaw (std::fmod (movement.yaw (), 360.0f));
    movement.pitch (std::fmod (movement.pitch (), 360.0f));
    movement.roll (std::fmod (movement.roll (), 360.0f));

    last_movement_ = movement;
    movement_mutex_.unlock ();

    return movement;
}
