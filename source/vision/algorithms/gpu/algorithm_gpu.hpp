// algorithm_gpu.hpp

#ifndef ALGORITHM_GPU_HPP
#define ALGORITHM_GPU_HPP

#include <QFile>
#include <QLoggingCategory>
#include <QMutex>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QVideoFrame>

#include "../algorithm_interface.hpp"
#include "../utils/frame_helper.hpp"
#include "../utils/operators.hpp"
#include "shared/movement3d/movement3d.hpp"
#include "shared/movement3d/movement3d_filter.hpp"

Q_DECLARE_LOGGING_CATEGORY (visionAlgorithmGpuLog)

class AlgorithmGpu final : public AlgorithmInterface, protected QOpenGLExtraFunctions {
    public:
    AlgorithmGpu ();
    ~AlgorithmGpu () final = default;

    [[nodiscard]] int MaxDebugLevel () const final;
    [[nodiscard]] int DebugLevel () const final;
    void SetDebugLevel (const int& new_level) final;

    void SetReference () final;
    FrameData Execute (const QVideoFrame& const_buffer) final;

    private:
    const size_t kImageProcessingHeight = 400;
    const size_t kImageProcessingWidth  = 700;

    void GenerateTextures ();
    void GenerateFramebuffer ();
    void GenerateVertexbuffer ();
    void CompileShaders ();
    void RenderSetup ();
    void RenderCleanup ();
    void DownscaleAndBlur (GLuint texture_handle);
    void Segmentation (image_t& image);
    void CalculateHistogram (std::vector<int>& histogram);
    int CalculateThreshold (const std::vector<int>& histogram);
    bool Extraction (image_t& image, Movement3D& movement);

    FrameHelper frame_helper_;
    const int max_debug_level_{ 2 };
    int debug_level_{ 0 };
    bool background_is_grayscale_;
    GLuint background_tex_;
    GLuint framebuffer_;
    GLuint blurred_image_texture_;
    GLuint segmented_image_texture_;
    GLuint background_vao_;
    GLuint background_vbo_;
    QOpenGLShaderProgram blur_program_;
    QOpenGLShaderProgram segmentation_program_;
    points_t markers_;
    points_t reference_;
    operators operators_;
    QMutex markers_mutex_;
    Movement3D last_movement_;
    Movement3DFilter movement3d_average_;
};

#endif // ALGORITHM_GPU_HPP
