#include "algorithm_original.hpp"
#include "operators/operators.hpp"

AlgorithmOriginal::AlgorithmOriginal (QOpenGLContext& opengl_context,
AugmentationWidget& augmentation)
: VisionAlgorithm (3, opengl_context, augmentation)
, last_movement_ ()
, movement3d_average_ (1) {
}

AlgorithmOriginal::~AlgorithmOriginal () {
}
void AlgorithmOriginal::SetReference () {
    markers_mutex_.lock ();
    reference_ = markers_;
    markers_mutex_.unlock ();
}

movement3d AlgorithmOriginal::Execute (const QVideoFrame& const_buffer) {
    bool status = true;
    movement3d movement;
    image_t image;

    status = FrameToRam (const_buffer, image);

    if (status) {
        status = Process (image, movement);

        free (image.data);
    }

    if (status) {
        last_movement_ = movement;
    } else {
        movement = last_movement_;
    }

    return movement;
}


bool AlgorithmOriginal::Process (image_t& image, movement3d& movement) {
    // start image processing
    operators_.preprocessing (image);
    if (debug_level_ == 1) {

        SetBackground (image);
    }

    operators_.segmentation (image);
    if (debug_level_ == 2) {
        SetBackground (image);
    }

    markers_mutex_.lock ();
    markers_.clear ();
    operators_.extraction (image, markers_);
    if (debug_level_ == 3) {
        SetBackground (image);
    }

    bool is_clasified = operators_.classification (reference_, markers_, movement); // classify
    if (is_clasified) {
        movement                  = movement3d_average_.average (movement);
        translation_t translation = movement.translation ();
        movement.translation (
        { movement.translation_delta_to_absolute (translation.x, image.width, -1, 1),
        movement.translation_delta_to_absolute (translation.y, image.height, -1, 1) });
    }

    markers_mutex_.unlock ();

    return is_clasified;
}
