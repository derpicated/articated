// algorithm_gpu.hpp

#ifndef ALGORITHM_GPU_HPP
#define ALGORITHM_GPU_HPP

#include <QFile>
#include <QMutex>
#include <QOffscreenSurface>
#include <QOpenGLShaderProgram>
#include <QVideoFrame>

#include "../vision_algorithm.hpp"
#include "movement3d/movement3d.hpp"
#include "movement3d/movement3d_filter.hpp"

class algorithm_gpu : public vision_algorithm {
    Q_OBJECT

    public:
    algorithm_gpu (QOpenGLContext& opengl_context, augmentation_widget& augmentation);
    ~algorithm_gpu ();

    void set_reference () override;
    movement3d execute (const QVideoFrame& const_buffer) override;

    private:
    const size_t IMAGE_PROCESSING_HEIGHT    = 400;
    const size_t IMAGE_PROCESSING_WIDTH_MAX = 700;

    void generate_textures ();
    void generate_framebuffer ();
    void generate_vertexbuffer ();
    void compile_shaders ();
    void downscale_and_blur (GLuint texture_handle);
    void segmentation (image_t& image);
    void calculate_histogram (std::vector<int>& histogram);
    int calculate_threshold (const std::vector<int>& histogram);
    bool extraction (image_t& image, movement3d& movement);

    GLuint _framebuffer;
    GLuint _blurred_image_texture;
    GLuint _segmented_image_texture;
    QOffscreenSurface _dummy_surface;
    GLuint _background_vao;
    GLuint _background_vbo;
    QOpenGLShaderProgram _blur_program;
    QOpenGLShaderProgram _segmentation_program;
    points_t _markers;
    points_t _reference;
    operators _operators;
    QMutex _markers_mutex;
    movement3d _last_movement;
    movement3d_average _movement3d_average;
};

#endif // ALGORITHM_GPU_HPP
