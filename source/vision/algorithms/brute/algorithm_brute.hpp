// algorithm_brute.hpp

#ifndef ALGORITHM_BRUTE_HPP
#define ALGORITHM_BRUTE_HPP

#include <QFile>
#include <QMutex>
#include <QOpenGLShaderProgram>
#include <QVideoFrame>
#include <optional>

#include "../shared/operators.hpp"
#include "../vision_algorithm.hpp"
#include "shared/movement3d/movement3d.hpp"
#include "shared/movement3d/movement3d_filter.hpp"

class AlgorithmBrute : public VisionAlgorithm {
    Q_OBJECT

    public:
    AlgorithmBrute ();
    ~AlgorithmBrute ();

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
    void Extraction (image_t& image);
    void CalculateFibonacciAngles ();
    void CalculateMarkerPredictions ();
    std::optional<Movement3D> GetBestPrediction (const points_t& markers, image_t& image);
    std::optional<Movement3D> Classification (image_t& image);

    int number_of_angles_{ 1000 };
    GLuint framebuffer_;
    GLuint blurred_image_texture_;
    GLuint segmented_image_texture_;
    GLuint background_vao_;
    GLuint background_vbo_;
    QOpenGLShaderProgram blur_program_;
    QOpenGLShaderProgram segmentation_program_;
    points_t markers_;
    points_t reference_;
    std::vector<points_t> marker_predictions_;
    operators operators_;
    std::vector<float> fibonacci_angles_;
    QMutex markers_mutex_;
    Movement3D last_movement_;
    Movement3DFilter movement3d_average_;
};

#endif // ALGORITHM_BRUTE_HPP
