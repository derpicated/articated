#include "algorithm_gpu.hpp"
#include "operators/operators.hpp"

#include <iostream>

algorithm_gpu::algorithm_gpu (QOpenGLContext& opengl_context, augmentation_widget& augmentation)
: vision_algorithm (3, opengl_context, augmentation)
, _last_movement ()
, _movement3d_average (1) {
    // set up vision framebuffer collor attachement texture
    glGenTextures (1, &_framebuffer_texture);
    glBindTexture (GL_TEXTURE_2D, _framebuffer_texture);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, IMAGE_PROCESSING_WIDTH_MAX,
    IMAGE_PROCESSING_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture (GL_TEXTURE_2D, 0);

    // set up vision framebuffer
    GLuint previous_framebuffer;
    glGetIntegerv (GL_FRAMEBUFFER_BINDING, (GLint*)&previous_framebuffer);

    glGenFramebuffers (1, &_framebuffer);
    glBindFramebuffer (GL_FRAMEBUFFER, _framebuffer);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    _framebuffer_texture, 0);
    glBindFramebuffer (GL_FRAMEBUFFER, previous_framebuffer);
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

    // Create intermediate image for RAM based processing steps
    image_t image;
    image.height = IMAGE_PROCESSING_HEIGHT;
    image.width  = IMAGE_PROCESSING_WIDTH_MAX; // TODO Fix better aspectratio
    // A pixel size of 4 is used, as this fits everything from R up to RGBA
    image.data = (uint8_t*)malloc (image.width * image.height * 4);

    // Upload image to GPU if necessary
    status = frame_to_texture (const_buffer, texture_handle, format);

    if (status) {
        status = preprocess (texture_handle, format, image);
    }

    if (status) {
        status = process (image, movement);
    }

    if (status) {
        _last_movement = movement;
    } else {
        movement = _last_movement;
    }

    free (image.data);
    return movement;
}

bool algorithm_gpu::preprocess (GLuint texture_handle, GLuint format, image_t& image) {
    if (_debug_level == 0) {
        _augmentation.setBackground (texture_handle, false);
    }

    GLuint _previous_framebuffer;
    glGetIntegerv (GL_FRAMEBUFFER_BINDING, (GLint*)&_previous_framebuffer);
    glBindFramebuffer (GL_FRAMEBUFFER, _framebuffer);


    // start image processing

#warning "DO PREPROCEESSS"
    //_operators.preprocessing (image);
    if (_debug_level == 1) {
        _augmentation.setBackground (texture_handle, false);
    }

    switch (glCheckFramebufferStatus (GL_FRAMEBUFFER)) {
        case GL_FRAMEBUFFER_COMPLETE: {
            std::cout << "GL_FRAMEBUFFER_COMPLETE" << std::endl;
            break;
        }
        case GL_FRAMEBUFFER_UNDEFINED: {
            std::cout << "GL_FRAMEBUFFER_UNDEFINED" << std::endl;
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
            std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
            std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
            break;
        }
        case GL_FRAMEBUFFER_UNSUPPORTED: {
            std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: {
            std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
            break;
        }
        default: {
            std::cout << glCheckFramebufferStatus (GL_FRAMEBUFFER) << std::endl;
            break;
        }
    }

    glReadPixels (
    0, 0, image.width, image.height, format, GL_UNSIGNED_BYTE, image.data);

    // _operators.segmentation (image);
    if (_debug_level == 2) {
        set_background (image);
    }

    glReadPixels (
    0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    glBindFramebuffer (GL_FRAMEBUFFER, _previous_framebuffer);

    return false;
}


bool algorithm_gpu::process (image_t& image, movement3d& movement) {
    _markers_mutex.lock ();
    _markers.clear ();
    _operators.extraction (image, _markers);
    if (_debug_level == 3) {
        set_background (image);
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
