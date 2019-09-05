#include "algorithm_gpu.hpp"

#include <iostream>

#include "operators/operators.hpp"

algorithm_gpu::algorithm_gpu (QOpenGLContext& opengl_context, augmentation_widget& augmentation)
: vision_algorithm (3, opengl_context, augmentation)
, _last_movement ()
, _movement3d_average (1) {
    Q_INIT_RESOURCE (vision_gpu_shaders);
    _dummy_surface.create ();
    _opengl_context.makeCurrent (&_dummy_surface);
    generate_textures ();
    compile_shaders ();
    generate_framebuffer ();
    generate_vertexbuffer ();
    _opengl_context.doneCurrent ();
}

algorithm_gpu::~algorithm_gpu () {
}

void algorithm_gpu::generate_textures () {
    // set up blurred image texture
    glGenTextures (1, &_blurred_image_texture);
    glBindTexture (GL_TEXTURE_2D, _blurred_image_texture);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_R8, IMAGE_PROCESSING_WIDTH_MAX,
    IMAGE_PROCESSING_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glBindTexture (GL_TEXTURE_2D, 0);

    // set up segmented image texture
    glGenTextures (1, &_segmented_image_texture);
    glBindTexture (GL_TEXTURE_2D, _segmented_image_texture);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_R8, IMAGE_PROCESSING_WIDTH_MAX,
    IMAGE_PROCESSING_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glBindTexture (GL_TEXTURE_2D, 0);
}

void algorithm_gpu::generate_framebuffer () {
    // set up vision framebuffer
    glGenFramebuffers (1, &_framebuffer);
}

void algorithm_gpu::generate_vertexbuffer () {
    glGenVertexArrays (1, &_background_vao);
    glGenBuffers (1, &_background_vbo);

    glBindVertexArray (_background_vao);
    glBindBuffer (GL_ARRAY_BUFFER, _background_vbo);

    int pos_location = _segmentation_program.attributeLocation ("position");
    glVertexAttribPointer (pos_location, 2, GL_FLOAT, GL_FALSE,
    4 * sizeof (float), reinterpret_cast<void*> (0));
    glEnableVertexAttribArray (pos_location);

    int tex_location = _segmentation_program.attributeLocation ("tex");
    glVertexAttribPointer (tex_location, 2, GL_FLOAT, GL_FALSE,
    4 * sizeof (float), reinterpret_cast<void*> (2 * sizeof (float)));
    glEnableVertexAttribArray (tex_location);

    // fill buffer with data
    GLfloat interleaved_background_buff[6 * 4] = {
        -1.0, 1.0,  // poly 1 a
        0.0, 0.0,   // poly 1 a tex
        -1.0, -1.0, // poly 1 b
        0.0, 1.0,   // poly 1 b tex
        1.0, 1.0,   // poly 1 c
        1.0, 0.0,   // poly 1 c tex
        1.0, 1.0,   // poly 2 a
        1.0, 0.0,   // poly 2 a tex
        -1.0, -1.0, // poly 2 b
        0.0, 1.0,   // poly 2 b tex
        1.0, -1.0,  // poly 2 c
        1.0, 1.0    // poly 2 c tex
    };
    glBufferData (GL_ARRAY_BUFFER, sizeof (float) * 6 * 4,
    interleaved_background_buff, GL_STATIC_DRAW);
}

void algorithm_gpu::compile_shaders () {
    {
        QFile vs_file (":/vision_gpu_shaders/passthrough_vs.glsl");
        QFile fs_file (":/vision_gpu_shaders/blur_fs.glsl");
        vs_file.open (QIODevice::ReadOnly);
        fs_file.open (QIODevice::ReadOnly);
        QByteArray vs_source = vs_file.readAll ();
        QByteArray fs_source = fs_file.readAll ();

        if (QOpenGLContext::currentContext ()->isOpenGLES ()) {
            vs_source.prepend (QByteArrayLiteral ("#version 300 es\n"));
            fs_source.prepend (QByteArrayLiteral ("#version 300 es\n"));
        } else {
            vs_source.prepend (QByteArrayLiteral ("#version 410\n"));
            fs_source.prepend (QByteArrayLiteral ("#version 410\n"));
        }

        _blur_program.addShaderFromSourceCode (QOpenGLShader::Vertex, vs_source);
        _blur_program.addShaderFromSourceCode (QOpenGLShader::Fragment, fs_source);
        _blur_program.link ();

        _blur_program.bind ();
        _blur_program.setUniformValue ("u_tex_background", 0);
        _blur_program.release ();
    }
    {
        QFile vs_file (":/vision_gpu_shaders/passthrough_vs.glsl");
        QFile fs_file (":/vision_gpu_shaders/segment_fs.glsl");
        vs_file.open (QIODevice::ReadOnly);
        fs_file.open (QIODevice::ReadOnly);
        QByteArray vs_source = vs_file.readAll ();
        QByteArray fs_source = fs_file.readAll ();

        if (QOpenGLContext::currentContext ()->isOpenGLES ()) {
            vs_source.prepend (QByteArrayLiteral ("#version 300 es\n"));
            fs_source.prepend (QByteArrayLiteral ("#version 300 es\n"));
        } else {
            vs_source.prepend (QByteArrayLiteral ("#version 410\n"));
            fs_source.prepend (QByteArrayLiteral ("#version 410\n"));
        }

        _segmentation_program.addShaderFromSourceCode (QOpenGLShader::Vertex, vs_source);
        _segmentation_program.addShaderFromSourceCode (QOpenGLShader::Fragment, fs_source);
        _segmentation_program.link ();

        _segmentation_program.bind ();
        _segmentation_program.setUniformValue ("u_tex_background", 0);
        _segmentation_program.release ();
    }
}

void algorithm_gpu::set_reference () {
    _markers_mutex.lock ();
    _reference = _markers;
    _markers_mutex.unlock ();
}

movement3d algorithm_gpu::execute (const QVideoFrame& const_buffer) {
    _opengl_context.makeCurrent (&_dummy_surface);
    bool status = true;
    movement3d movement;
    GLuint texture_handle = 0;
    GLuint format         = GL_RED;

    // Create intermediate image for RAM based processing steps
    image_t image;
    image.height = IMAGE_PROCESSING_HEIGHT;
    image.width  = IMAGE_PROCESSING_WIDTH_MAX; // TODO Fix better aspectratio
    // A pixel size of 4 is used, as this fits everything from R up to RGBA
    image.data = (uint8_t*)malloc (image.width * image.height * 4);

    // Upload image to GPU if necessary
    status = frame_to_texture (const_buffer, texture_handle, format);
    if (_debug_level == 0) {
        _augmentation.setBackground (texture_handle, false);
    }

    downscale_and_blur (texture_handle);
    segmentation (image);
    // status = extraction (image, movement);

    if (status) {
        _last_movement = movement;
    } else {
        movement = _last_movement;
    }

    _opengl_context.doneCurrent ();
    free (image.data);
    return movement;
}

void algorithm_gpu::downscale_and_blur (GLuint texture_handle) {
    GLuint _previous_framebuffer;
    glGetIntegerv (GL_FRAMEBUFFER_BINDING, (GLint*)&_previous_framebuffer);
    glBindFramebuffer (GL_FRAMEBUFFER, _framebuffer);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    _blurred_image_texture, 0);
    glViewport (0, 0, IMAGE_PROCESSING_WIDTH_MAX, IMAGE_PROCESSING_HEIGHT);

    glBindVertexArray (_background_vao);
    _blur_program.bind ();
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, texture_handle);
    glDrawArrays (GL_TRIANGLES, 0, 6);
    glBindTexture (GL_TEXTURE_2D, 0);
    _blur_program.release ();
    glBindVertexArray (0);

    if (_debug_level == 1) {
        _augmentation.setBackground (_blurred_image_texture, true);
    }
}

void algorithm_gpu::segmentation (image_t& image) {
    GLuint _previous_framebuffer;
    glGetIntegerv (GL_FRAMEBUFFER_BINDING, (GLint*)&_previous_framebuffer);
    glBindFramebuffer (GL_FRAMEBUFFER, _framebuffer);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    _segmented_image_texture, 0);
    glViewport (0, 0, IMAGE_PROCESSING_WIDTH_MAX, IMAGE_PROCESSING_HEIGHT);

    glBindVertexArray (_background_vao);
    _segmentation_program.bind ();
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, _blurred_image_texture);
    glDrawArrays (GL_TRIANGLES, 0, 6);
    glBindTexture (GL_TEXTURE_2D, 0);
    _segmentation_program.release ();
    glBindVertexArray (0);

    if (_debug_level == 2) {
        _augmentation.setBackground (_segmented_image_texture, false);
    }

    glReadPixels (
    0, 0, image.width, image.height, GL_RED, GL_UNSIGNED_BYTE, image.data);
    image.format = GREY8;
    glBindFramebuffer (GL_FRAMEBUFFER, _previous_framebuffer);
}


bool algorithm_gpu::extraction (image_t& image, movement3d& movement) {
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
