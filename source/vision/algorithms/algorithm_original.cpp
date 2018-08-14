#include "algorithm_original.hpp"
#include "operators/operators.hpp"

algorithm_original::algorithm_original ()
: vision_algorithm (3)
, _last_movement ()
, _movement3d_average (1) {
}

algorithm_original::~algorithm_original () {
}
void algorithm_original::set_reference () {
    _markers_mutex.lock ();
    _reference = _markers;
    _markers_mutex.unlock ();
}

movement3d algorithm_original::execute (const QVideoFrame& const_buffer) {
    bool status = true;
    movement3d movement;
    image_t image;

    status = frame_to_ram (const_buffer, image);

    if (status) {
        status = process (image, movement);

        free (image.data);
    }

    if (status) {
        _last_movement = movement;
    } else {
        movement = _last_movement;
    }

    return movement;
}

bool algorithm_original::process (image_t& image, movement3d& movement) {
    // start image processing
    _operators.preprocessing (image);
    if (_debug_level == 1) {
        augmentation_widget::instance ().setBackground (image);
    }

    _operators.segmentation (image);
    if (_debug_level == 2) {
        augmentation_widget::instance ().setBackground (image);
    }

    _markers_mutex.lock ();
    _markers.clear ();
    _operators.extraction (image, _markers);
    if (_debug_level == 3) {
        augmentation_widget::instance ().setBackground (image);
    }

    bool is_clasified = _operators.classification (_reference, _markers, movement); // classify
    if (is_clasified) {
        movement                  = _movement3d_average.average (movement);
        translation_t translation = movement.translation ();
        movement.translation (
        { movement.translation_delta_to_absolute (translation.x, image.width, -1, 1),
        movement.translation_delta_to_absolute (translation.y, image.height, -1, 1) });
    }

    _markers_mutex.unlock ();

    return is_clasified;
}
