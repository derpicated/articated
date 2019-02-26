#include "algorithm_gpu.hpp"
#include "operators/operators.hpp"

algorithm_gpu::algorithm_gpu (QOpenGLContext& opengl_context, augmentation_widget& augmentation)
: vision_algorithm (3, opengl_context, augmentation)
, _last_movement ()
, _movement3d_average (1) {
}

algorithm_gpu::~algorithm_gpu () {
}

void algorithm_gpu::set_reference () {
    _markers_mutex.lock ();
    _reference = _markers;
    _markers_mutex.unlock ();
}

movement3d algorithm_gpu::execute (const QVideoFrame& const_buffer) {
    bool status = true;
    movement3d movement;
    GLuint texture_handle = 0;
    GLuint format;

    status = frame_to_texture (const_buffer, texture_handle, format);

    if (status) {
        status = process (texture_handle, format, movement);
    }

    if (status) {
        _last_movement = movement;
    } else {
        movement = _last_movement;
    }

    return movement;
}


bool algorithm_gpu::process (GLuint texture_handle, GLuint format, movement3d& movement) {
    if (_debug_level == 0) {
        _augmentation.setBackground (texture_handle, false);
    }

    // start image processing

#warning "DO PREPROCEESSS"
    //_operators.preprocessing (image);
    if (_debug_level == 1) {
        _augmentation.setBackground (texture_handle, false);
    }

    // create intermediate image for RAM based processing steps
    image_t image;
    image.height = IMAGE_PROCESSING_HEIGHT;
    image.width  = IMAGE_PROCESSING_WIDTH_MAX; // TODO Fix better aspectratio
    // A pixel size of 4 is used, as this fits everything from R up to RGBA
    image.data = (uint8_t*)malloc (image.width * image.height * 4);

    glReadPixels (
    0, 0, image.width, image.height, format, GL_UNSIGNED_BYTE, image.data);

    _operators.segmentation (image);
    if (_debug_level == 2) {
        set_background (image);
    }

    _markers_mutex.lock ();
    _markers.clear ();
    _operators.extraction (image, _markers);
    if (_debug_level == 3) {
        set_background (image);
    }
    free (image.data);

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
