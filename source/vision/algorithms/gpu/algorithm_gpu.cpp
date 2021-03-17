#include "algorithm_gpu.hpp"

#include <iostream>

Q_LOGGING_CATEGORY (visionAlgorithmGpuLog, "vision.algorithm.gpu", QtInfoMsg)

AlgorithmGpu::AlgorithmGpu ()
: AlgorithmInterface ()
, QOpenGLExtraFunctions () {
    Q_INIT_RESOURCE (vision_gpu_shaders);
    initializeOpenGLFunctions ();
    GenerateTextures ();
    CompileShaders ();
    GenerateFramebuffer ();
    GenerateVertexbuffer ();
}

int AlgorithmGpu::MaxDebugLevel () const {
    return max_debug_level_;
}

int AlgorithmGpu::DebugLevel () const {
    return debug_level_;
}

void AlgorithmGpu::SetDebugLevel (const int& new_level) {
    int level    = new_level;
    level        = level < 0 ? 0 : level;
    level        = level > max_debug_level_ ? max_debug_level_ : level;
    debug_level_ = level;
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

FrameData AlgorithmGpu::Execute (const QVideoFrame& const_buffer) {
    FrameData frame_data;
    Movement3D movement;
    GLuint texture_handle = 0;

    // Create intermediate image for RAM based processing steps
    image_t image;
    image.height = kImageProcessingHeight;
    image.width  = kImageProcessingWidth; // TODO Fix better aspectratio
    // A pixel size of 4 is used, as this fits everything from R up to RGBA
    image.data = (uint8_t*)malloc (image.width * image.height * 4);

    // Upload image to GPU if necessary
    std::optional<GLuint> optional_texture = frame_helper_.FrameToTexture (const_buffer);
    if (optional_texture) {
        texture_handle = optional_texture.value ();
    } else {
        qCWarning (visionAlgorithmGpuLog, "Could not upload frame to texture");
    }

    if (debug_level_ == 0) {
        frame_data["background"]              = texture_handle;
        frame_data["background_is_grayscale"] = false;
    }

    RenderSetup ();
    DownscaleAndBlur (texture_handle);
    if (debug_level_ == 1) {
        frame_data["background"]              = blurred_image_texture_;
        frame_data["background_is_grayscale"] = true;
    }

    Segmentation (image);
    if (debug_level_ == 2) {
        frame_data["background"]              = segmented_image_texture_;
        frame_data["background_is_grayscale"] = true;
    }
    RenderCleanup ();

    qCDebug (visionAlgorithmGpuLog, "Output texture: %u", texture_handle);

    if (Extraction (image, movement)) {
        last_movement_ = movement;
    } else {
        movement = last_movement_;
    }
    frame_data["transform"] = movement;

    free (image.data);

    return frame_data;
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

bool AlgorithmGpu::Extraction (image_t& image, Movement3D& movement) {
    markers_mutex_.lock ();
    markers_.clear ();
    operators_.remove_border_blobs (image, FOUR);
    operators_.extraction (image, markers_);
    if (debug_level_ == 3) {
        background_tex_ = frame_helper_.UploadImage (image, background_is_grayscale_);
    }

    bool is_classified = operators_.classification (reference_, markers_, movement); // classify
    if (is_classified) {
        movement                  = movement3d_average_.average (movement);
        translation_t translation = movement.translation ();
        movement.translation (
        { movement.translation_delta_to_absolute (translation.x, image.width, -1, 1),
        movement.translation_delta_to_absolute (translation.y, image.height, -1, 1) });
    }

    markers_mutex_.unlock ();

    return is_classified;
}
