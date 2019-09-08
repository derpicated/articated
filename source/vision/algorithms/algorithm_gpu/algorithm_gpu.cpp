#include "algorithm_gpu.hpp"

#include <iostream>

#include "operators/operators.hpp"

AlgorithmGpu::AlgorithmGpu (QOpenGLContext& opengl_context, augmentation_widget& augmentation)
: VisionAlgorithm (3, opengl_context, augmentation)
, last_movement_ ()
, movement3d_average_ (1) {
    Q_INIT_RESOURCE (vision_gpu_shaders);
    opengl_context_.makeCurrent (&dummy_surface_);
    GenerateTextures ();
    CompileShaders ();
    GenerateFramebuffer ();
    GenerateVertexbuffer ();
    opengl_context_.doneCurrent ();
}

AlgorithmGpu::~AlgorithmGpu () {
}

void AlgorithmGpu::GenerateTextures () {
    // set up blurred image texture
    glGenTextures (1, &blurred_image_texture_);
    glBindTexture (GL_TEXTURE_2D, blurred_image_texture_);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_R8, kImageProcessingWidth,
    kImageProcessingHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glBindTexture (GL_TEXTURE_2D, 0);

    // set up segmented image texture
    glGenTextures (1, &segmented_image_texture_);
    glBindTexture (GL_TEXTURE_2D, segmented_image_texture_);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_R8, kImageProcessingWidth,
    kImageProcessingHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glBindTexture (GL_TEXTURE_2D, 0);
}

void AlgorithmGpu::GenerateFramebuffer () {
    // set up vision framebuffer
    glGenFramebuffers (1, &framebuffer_);
}

void AlgorithmGpu::GenerateVertexbuffer () {
    glGenVertexArrays (1, &background_vao_);
    glGenBuffers (1, &background_vbo_);

    glBindVertexArray (background_vao_);
    glBindBuffer (GL_ARRAY_BUFFER, background_vbo_);

    int pos_location = segmentation_program_.attributeLocation ("position");
    glVertexAttribPointer (pos_location, 2, GL_FLOAT, GL_FALSE,
    4 * sizeof (float), reinterpret_cast<void*> (0));
    glEnableVertexAttribArray (pos_location);

    int tex_location = segmentation_program_.attributeLocation ("tex");
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

void AlgorithmGpu::CompileShaders () {
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

        blur_program_.addShaderFromSourceCode (QOpenGLShader::Vertex, vs_source);
        blur_program_.addShaderFromSourceCode (QOpenGLShader::Fragment, fs_source);
        blur_program_.link ();

        blur_program_.bind ();
        blur_program_.setUniformValue ("u_tex_background", 0);
        blur_program_.release ();
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

        segmentation_program_.addShaderFromSourceCode (QOpenGLShader::Vertex, vs_source);
        segmentation_program_.addShaderFromSourceCode (QOpenGLShader::Fragment, fs_source);
        segmentation_program_.link ();

        segmentation_program_.bind ();
        segmentation_program_.setUniformValue ("u_tex_background", 0);
        segmentation_program_.release ();
    }
}

void AlgorithmGpu::SetReference () {
    markers_mutex_.lock ();
    reference_ = markers_;
    markers_mutex_.unlock ();
}

movement3d AlgorithmGpu::Execute (const QVideoFrame& const_buffer) {
    opengl_context_.makeCurrent (&dummy_surface_);
    bool status = true;
    movement3d movement;
    GLuint texture_handle = 0;
    GLuint format         = GL_RED;

    // Create intermediate image for RAM based processing steps
    image_t image;
    image.height = kImageProcessingHeight;
    image.width  = kImageProcessingWidth; // TODO Fix better aspectratio
    // A pixel size of 4 is used, as this fits everything from R up to RGBA
    image.data = (uint8_t*)malloc (image.width * image.height * 4);

    // Upload image to GPU if necessary
    status = FrameToTexture (const_buffer, texture_handle, format);
    if (debug_level_ == 0) {
        augmentation_.setBackground (texture_handle, false);
    }

    RenderSetup ();
    DownscaleAndBlur (texture_handle);
    Segmentation (image);
    RenderCleanup ();

    status = Extraction (image, movement);

    if (status) {
        last_movement_ = movement;
    } else {
        movement = last_movement_;
    }

    opengl_context_.doneCurrent ();
    free (image.data);
    return movement;
}

void AlgorithmGpu::RenderSetup () {
    glBindFramebuffer (GL_FRAMEBUFFER, framebuffer_);
    glViewport (0, 0, kImageProcessingWidth, kImageProcessingHeight);
    glActiveTexture (GL_TEXTURE0);
    glBindVertexArray (background_vao_);
}

void AlgorithmGpu::RenderCleanup () {
    glBindVertexArray (0);
    glBindFramebuffer (GL_FRAMEBUFFER, 0);
    glUseProgram (0);
}


void AlgorithmGpu::DownscaleAndBlur (GLuint texture_handle) {
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    blurred_image_texture_, 0);
    glBindTexture (GL_TEXTURE_2D, texture_handle);
    blur_program_.bind ();
    glDrawArrays (GL_TRIANGLES, 0, 6);

    if (debug_level_ == 1) {
        augmentation_.setBackground (blurred_image_texture_, true);
    }
}

void AlgorithmGpu::Segmentation (image_t& image) {
    std::vector<int> histogram;
    histogram.resize (UINT8_MAX + 1, 0);
    CalculateHistogram (histogram);
    float threshold = static_cast<float> (CalculateThreshold (histogram)) / 255;

    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    segmented_image_texture_, 0);
    glBindTexture (GL_TEXTURE_2D, blurred_image_texture_);
    segmentation_program_.bind ();
    segmentation_program_.setUniformValue ("u_threshold", threshold);
    glDrawArrays (GL_TRIANGLES, 0, 6);

    if (debug_level_ == 2) {
        augmentation_.setBackground (segmented_image_texture_, false);
    }

    glReadPixels (
    0, 0, image.width, image.height, GL_RED, GL_UNSIGNED_BYTE, image.data);
    image.format = GREY8;
}

void AlgorithmGpu::CalculateHistogram (std::vector<int>& histogram) {
    std::vector<uint8_t> pixels;
    pixels.resize (kImageProcessingHeight * kImageProcessingWidth);

    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    blurred_image_texture_, 0);
    glReadPixels (0, 0, kImageProcessingWidth, kImageProcessingHeight, GL_RED,
    GL_UNSIGNED_BYTE, pixels.data ());

    for (int pixel : pixels) {
        ++histogram[pixel];
    }
}

int AlgorithmGpu::CalculateThreshold (const std::vector<int>& histogram) {
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

bool AlgorithmGpu::Extraction (image_t& image, movement3d& movement) {
    markers_mutex_.lock ();
    markers_.clear ();
    operators_.remove_border_blobs (image, FOUR);
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
