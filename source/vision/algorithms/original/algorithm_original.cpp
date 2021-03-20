#include "algorithm_original.hpp"

void AlgorithmOriginal::SetReference () {
    markers_mutex_.lock ();
    reference_ = markers_;
    markers_mutex_.unlock ();
}

int AlgorithmOriginal::MaxDebugLevel () const {
    return max_debug_level_;
}

int AlgorithmOriginal::DebugLevel () const {
    return debug_level_;
}

void AlgorithmOriginal::SetDebugLevel (const int& new_level) {
    int level    = new_level;
    level        = level < 0 ? 0 : level;
    level        = level > max_debug_level_ ? max_debug_level_ : level;
    debug_level_ = level;
}

void AlgorithmOriginal::SetBackground (image_t image) {
    bool is_grayscale;
    background_tex_          = frame_helper_.UploadImage (image, is_grayscale);
    background_is_grayscale_ = is_grayscale;
}

FrameData AlgorithmOriginal::Execute (const QVideoFrame& const_buffer) {
    bool status = true;
    Movement3D movement;
    image_t image;

    status = frame_helper_.FrameToRam (const_buffer, image, true, background_tex_);

    if (status) {
        status = Process (image, movement);

        free (image.data);
    }

    if (status) {
        last_movement_ = movement;
    } else {
        movement = last_movement_;
    }

    return { { "transform", movement }, { "background", background_tex_ },
        { "backgroundIsGrayscale", background_is_grayscale_ } };
}


bool AlgorithmOriginal::Process (image_t& image, Movement3D& movement) {
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
