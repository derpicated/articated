#include "algorithm_gpu.hpp"

#include <iostream>

#include "operators/operators.hpp"

algorithm_gpu::algorithm_gpu (QOpenGLContext& opengl_context, augmentation_widget& augmentation)
: vision_algorithm (3, opengl_context, augmentation)
, _last_movement ()
, _movement3d_average (1) {
    Q_INIT_RESOURCE (vision_gpu_shaders);
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

    render_setup ();
    downscale_and_blur (texture_handle);
    segmentation (image);
    render_cleanup ();

    status = extraction (image, movement);

    if (status) {
        _last_movement = movement;
    } else {
        movement = _last_movement;
    }

    _opengl_context.doneCurrent ();
    free (image.data);
    return movement;
}

void algorithm_gpu::render_setup () {
    glBindFramebuffer (GL_FRAMEBUFFER, _framebuffer);
    glViewport (0, 0, IMAGE_PROCESSING_WIDTH_MAX, IMAGE_PROCESSING_HEIGHT);
    glActiveTexture (GL_TEXTURE0);
    glBindVertexArray (_background_vao);
}

void algorithm_gpu::render_cleanup () {
    glBindVertexArray (0);
    glBindFramebuffer (GL_FRAMEBUFFER, 0);
    glUseProgram (0);
}


void algorithm_gpu::downscale_and_blur (GLuint texture_handle) {
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    _blurred_image_texture, 0);
    glBindTexture (GL_TEXTURE_2D, texture_handle);
    _blur_program.bind ();
    glDrawArrays (GL_TRIANGLES, 0, 6);

    if (_debug_level == 1) {
        _augmentation.setBackground (_blurred_image_texture, true);
    }
}

void algorithm_gpu::segmentation (image_t& image) {
    std::vector<int> histogram;
    histogram.resize (UINT8_MAX + 1, 0);
    calculate_histogram (histogram);
    float threshold = static_cast<float> (calculate_threshold (histogram)) / 255;

    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    _segmented_image_texture, 0);
    glBindTexture (GL_TEXTURE_2D, _blurred_image_texture);
    _segmentation_program.bind ();
    _segmentation_program.setUniformValue ("u_threshold", threshold);
    glDrawArrays (GL_TRIANGLES, 0, 6);

    if (_debug_level == 2) {
        _augmentation.setBackground (_segmented_image_texture, false);
    }

    glReadPixels (
    0, 0, image.width, image.height, GL_RED, GL_UNSIGNED_BYTE, image.data);
    image.format = GREY8;
}

void algorithm_gpu::calculate_histogram (std::vector<int>& histogram) {
    std::vector<uint8_t> pixels;
    pixels.resize (IMAGE_PROCESSING_HEIGHT * IMAGE_PROCESSING_WIDTH_MAX);

    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    _blurred_image_texture, 0);
    glReadPixels (0, 0, IMAGE_PROCESSING_WIDTH_MAX, IMAGE_PROCESSING_HEIGHT,
    GL_RED, GL_UNSIGNED_BYTE, pixels.data ());

    for (int pixel : pixels) {
        ++histogram[pixel];
    }
}

int algorithm_gpu::calculate_threshold (const std::vector<int>& histogram) {
    const int pValues = 256;
    uint8_t lPixel    = 255;
    uint8_t hPixel    = 0;
    int T             = 0; // mean between bright and dark
    int Told          = 0; // old mean
    int cnt           = 0; // some random counter for... counting
    // find hPixel
    for (cnt = pValues; cnt != 0; --cnt) {
        if (histogram[cnt - 1]) {
            hPixel = cnt - 1;
            cnt    = 1; // not 0 because for loop
        }
    }
    // find lPixel
    for (cnt = pValues; cnt != 0; --cnt) {
        if (histogram[pValues - cnt]) {
            lPixel = pValues - cnt;
            cnt    = 1; // not 0 because for loop
        }
    }
    // check for zero or same value
    if (lPixel == hPixel) {
        T = lPixel;
    } else {
        T = (int)(lPixel + hPixel) / 2 + 0.5; // center of pixels
        uint32_t meanDark   = 0;              // mean dark (from 0 to T)
        uint32_t meanBright = 0; // mean bright (from T to and including end)
        while (Told != T) {
            Told          = T;
            uint32_t pCnt = 0; // pixels
            // mean left (using Told)
            // 0 to Told
            meanDark = 0;
            pCnt     = 0;
            for (cnt = 0; cnt <= Told; ++cnt) {
                meanDark += cnt * histogram[cnt]; // pixel value
                pCnt += histogram[cnt];           // pixel count
            }
            meanDark /= pCnt;

            // mean right (using Told)
            // Told to end
            meanBright = 0;
            pCnt       = 0;
            for (cnt = 255; cnt > Told; --cnt) {
                meanBright += cnt * histogram[cnt]; // pixel value
                pCnt += histogram[cnt];             // pixel count
            }
            meanBright /= pCnt;
            // mean of means (rounded)
            T = (int)(meanDark + meanBright) / 2 + 0.5;
        }
    }

    return T;
}

bool algorithm_gpu::extraction (image_t& image, movement3d& movement) {
    _markers_mutex.lock ();
    _markers.clear ();
    _operators.remove_border_blobs (image, FOUR);
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
