// algorithm_gpu.hpp

#ifndef ALGORITHM_GPU_HPP
#define ALGORITHM_GPU_HPP

#include <QFile>
#include <QMutex>
#include <QOpenGLShaderProgram>
#include <QVideoFrame>

#include "../shared/operators.hpp"
#include "../vision_algorithm.hpp"
#include "shared/movement3d/movement3d.hpp"
#include "shared/movement3d/movement3d_filter.hpp"

class AlgorithmGpu : public VisionAlgorithm {
    Q_OBJECT

    public:
    AlgorithmGpu ();
    ~AlgorithmGpu ();

    void SetReference () override;
    FrameData Execute (const QVideoFrame& const_buffer) override;

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
