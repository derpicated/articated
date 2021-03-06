#include "algorithm_brute.hpp"

#include <cmath>
#include <iostream>

namespace {
constexpr float kTau         = M_PI * 2.0f;
constexpr float kGoldenRatio = (sqrt (5.0f) + 1.0f) / 2.0f;
constexpr float kGoldenAngle = (2.0f - kGoldenRatio) * kTau;

constexpr float kRadToDeg = 180.0f / M_PI;
constexpr float kDegToRad = M_PI / 180.0f;

float RadToDeg (float radians) {
    return radians * kRadToDeg;
}
float DegToRad (float degrees) {
    return degrees * kDegToRad;
}

float remap (float value, float from_start, float from_stop, float to_start, float to_stop) {
    return to_start + (to_stop - to_start) * ((value - from_start) / (from_stop - from_start));
}

point_t MarkersCentroid (const points_t& markers) {
    point_t centroid = { 0.0, 0.0 };
    for (const auto& marker : markers) {
        centroid.x += marker.second.x / markers.size ();
        centroid.y += marker.second.y / markers.size ();
    }
    return centroid;
}

#warning normalize propperly pls
// normalize markers around their combined centroid
points_t NormalizeMarkers (const points_t& markers) {
    points_t normalized_markers;
    point_t centroid = MarkersCentroid (markers);

    float max_vector_length = 0.0;
    for (const auto& marker : markers) {
        float normalized_x               = marker.second.x - centroid.x;
        float normalized_y               = marker.second.y - centroid.y;
        normalized_markers[marker.first] = { normalized_x, normalized_y };

        float vector_length =
        std::sqrt ((normalized_x * normalized_x) + (normalized_y * normalized_y));

        if (vector_length > max_vector_length) {
            max_vector_length = vector_length;
        }
    }
    qDebug () << "Vector is:" << max_vector_length;
    for (auto& marker : normalized_markers) {
        marker.second.x /= max_vector_length;
        marker.second.y /= max_vector_length;
    }

    return normalized_markers;
}

float CalculateMatchFactor (const points_t& prediction, const points_t& current) {
    float match_factor = 0; // lower is better!
    for (const auto& marker : current) {
        if (const auto& predicted_marker = prediction.find (marker.first);
            predicted_marker != prediction.end ()) {
            const float delta_x = marker.second.x - predicted_marker->second.x;
            const float delta_y = marker.second.y - predicted_marker->second.y;

            match_factor += std::sqrt ((delta_x * delta_x) + (delta_y * delta_y));
        }
    }

    return match_factor;
}
} // namespace

AlgorithmBrute::AlgorithmBrute ()
: VisionAlgorithm (4)
, last_movement_ ()
, movement3d_average_ (1) {
    Q_INIT_RESOURCE (vision_brute_shaders);
    GenerateTextures ();
    CompileShaders ();
    GenerateFramebuffer ();
    GenerateVertexbuffer ();
}

AlgorithmBrute::~AlgorithmBrute () {
}

void AlgorithmBrute::GenerateTextures () {
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

void AlgorithmBrute::GenerateFramebuffer () {
    // set up vision framebuffer
    glGenFramebuffers (1, &framebuffer_);
}

void AlgorithmBrute::GenerateVertexbuffer () {
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

void AlgorithmBrute::CompileShaders () {
    {
        QFile vs_file (":/vision_brute_shaders/passthrough_vs.glsl");
        QFile fs_file (":/vision_brute_shaders/blur_fs.glsl");
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
        QFile vs_file (":/vision_brute_shaders/passthrough_vs.glsl");
        QFile fs_file (":/vision_brute_shaders/segment_fs.glsl");
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

void AlgorithmBrute::SetReference () {
    markers_mutex_.lock ();
    reference_ = markers_;
    marker_predictions_.clear ();
    markers_mutex_.unlock ();
}

FrameData AlgorithmBrute::Execute (const QVideoFrame& const_buffer) {
    FrameData frame_data;
    GLuint texture_handle = 0;
    GLuint format         = GL_RED;

    // Create intermediate image for RAM based processing steps
    image_t image;
    image.height = kImageProcessingHeight;
    image.width  = kImageProcessingWidth; // TODO Fix better aspectratio
    // A pixel size of 4 is used, as this fits everything from R up to RGBA
    image.data = (uint8_t*)malloc (image.width * image.height * 4);

    // Upload image to GPU if necessary
    FrameToTexture (const_buffer, texture_handle, format);
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

    Extraction (image);
    if (debug_level_ == 3) {
        SetBackground (image);
        frame_data["background"]              = background_tex_;
        frame_data["background_is_grayscale"] = background_is_grayscale_;
    }

    auto transform = Classification (image);
    if (debug_level_ == 4) {
        // SetBackground (image);
        frame_data["background"]              = background_tex_;
        frame_data["background_is_grayscale"] = background_is_grayscale_;
    }

    if (transform.has_value ()) {
        last_movement_ = transform.value ();
    }

    frame_data["transform"] = last_movement_;

    free (image.data);

    return frame_data;
}

void AlgorithmBrute::RenderSetup () {
    glBindFramebuffer (GL_FRAMEBUFFER, framebuffer_);
    glViewport (0, 0, kImageProcessingWidth, kImageProcessingHeight);
    glActiveTexture (GL_TEXTURE0);
    glBindVertexArray (background_vao_);
}

void AlgorithmBrute::RenderCleanup () {
    glBindVertexArray (0);
    glBindFramebuffer (GL_FRAMEBUFFER, 0);
    glUseProgram (0);
}


void AlgorithmBrute::DownscaleAndBlur (GLuint texture_handle) {
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    blurred_image_texture_, 0);
    glBindTexture (GL_TEXTURE_2D, texture_handle);
    blur_program_.bind ();
    glDrawArrays (GL_TRIANGLES, 0, 6);
}

void AlgorithmBrute::Segmentation (image_t& image) {
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

void AlgorithmBrute::CalculateHistogram (std::vector<int>& histogram) {
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

int AlgorithmBrute::CalculateThreshold (const std::vector<int>& histogram) {
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

void AlgorithmBrute::Extraction (image_t& image) {
    markers_mutex_.lock ();
    markers_.clear ();
    operators_.remove_border_blobs (image, FOUR);
    operators_.extraction (image, markers_);
    markers_mutex_.unlock ();
}

void AlgorithmBrute::CalculateFibonacciAngles () {
    fibonacci_angles_.clear ();
    fibonacci_angles_.reserve (number_of_angles_ * 2);
    // float height_step = ;

    // use golden ratio to construct a fibonacci hemi-sphere of unit vectors
    for (int i = 0; i < number_of_angles_; ++i) {
        const float theta = fmodf ((kGoldenAngle * i), kTau);
        const float phi =
        acos (1 - (2 * static_cast<float> (i) / number_of_angles_)) - (M_PI / 2);
        fibonacci_angles_[i * 2]       = theta;
        fibonacci_angles_[(i * 2) + 1] = phi;
    }
}

void AlgorithmBrute::CalculateMarkerPredictions () {
    QMatrix4x4 projection_matrix;
    projection_matrix.ortho (-2.0f, +2.0f, -2.0f, +2.0f, 1.0f, 25.0f);

    // rotate each marker according to each vector from the fibonacci sphere
    for (int i = 0; i < number_of_angles_; ++i) {
        const float theta = RadToDeg (fibonacci_angles_[i * 2]);
        const float phi   = RadToDeg (fibonacci_angles_[(i * 2) + 1]);

        QMatrix4x4 rotation_matrix;
        rotation_matrix.rotate (phi, 1.0f, 0.0f);
        rotation_matrix.rotate (theta, 0.0f, 1.0f);

        QMatrix4x4 mult_mat = projection_matrix * rotation_matrix;
        points_t prediction;

        const auto normalized_markers = NormalizeMarkers (reference_);
        for (const auto& marker : normalized_markers) {
            auto id    = marker.first;
            auto point = QVector3D (marker.second.x, marker.second.y, 0.0f);
            QVector3D rotated_point = mult_mat * point;
            prediction[id] = { rotated_point.x (), rotated_point.y () };
        }
        marker_predictions_.push_back (prediction);

        // for (const auto& markers : marker_predictions_) {
        //     for (const auto& marker : markers) {
        //         qDebug () << marker.first;
        //     }
        // }
    }
}

std::optional<Movement3D>
AlgorithmBrute::GetBestPrediction (const points_t& markers, image_t& image) {
    const point_t centroid        = MarkersCentroid (markers);
    const auto normalized_markers = NormalizeMarkers (markers);
    int best_match_index          = -1;
    float best_match_factor       = 100000; // biga numba
    for (int i = 0; i < number_of_angles_; ++i) {
        float match_factor =
        CalculateMatchFactor (normalized_markers, marker_predictions_[i]);
        if (match_factor < best_match_factor) {
            qDebug () << "New best match" << i << " with factor " << match_factor;
            best_match_factor = match_factor;
            best_match_index  = i;
        }
    }

    Movement3D tmp;

    float theta = RadToDeg (fibonacci_angles_[best_match_index * 2]);
    float phi   = RadToDeg (fibonacci_angles_[(best_match_index * 2) + 1]);
    tmp.yaw (theta);
    tmp.pitch (phi);
    tmp.scale (1.0f);
    translation_t tmp_translation;
    tmp_translation.x = remap (centroid.x, 0.0, image.width, -1.0, 1.0);
    tmp_translation.y = remap (centroid.y, 0.0, image.height, 1.0, -1.0);
    tmp.translation (tmp_translation);

    if (debug_level_ == 4) {
        const auto& tmp_prediction = marker_predictions_[best_match_index];
        for (const auto& marker : tmp_prediction) {
            const int scale = std::min (image.width, image.height) / 2;
            const int x     = (marker.second.x * scale) + centroid.x;
            const int y     = (marker.second.y * scale) + centroid.y;

            if (2 <= x && x < image.width && 2 <= y && y < image.height) {
                qDebug () << x << y;
                // set a crosshair at marker position
                image.data[((y - 2) * image.width) + x] = 255;
                image.data[((y - 1) * image.width) + x] = 255;
                image.data[((y + 1) * image.width) + x] = 255;
                image.data[((y + 2) * image.width) + x] = 255;
                image.data[(y * image.width) + x - 2]   = 255;
                image.data[(y * image.width) + x - 1]   = 255;
                image.data[(y * image.width) + x + 1]   = 255;
                image.data[(y * image.width) + x + 2]   = 255;
            }
        }
        SetBackground (image);
    }

    return tmp; // std::nullopt;
}

std::optional<Movement3D> AlgorithmBrute::Classification (image_t& image) {
    if (marker_predictions_.empty ()) {
        if (fibonacci_angles_.empty ()) {
            CalculateFibonacciAngles ();
        }
        CalculateMarkerPredictions ();
    }

    return GetBestPrediction (markers_, image);
}
