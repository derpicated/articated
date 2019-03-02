// algorithm_gpu.hpp

#ifndef ALGORITHM_GPU_HPP
#define ALGORITHM_GPU_HPP

#include <QMutex>
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

    bool preprocess (GLuint texture_handle, GLuint format, image_t& image);
    bool process (image_t& image, movement3d& movement);

    GLuint _framebuffer;
    points_t _markers;
    points_t _reference;
    operators _operators;
    QMutex _markers_mutex;
    movement3d _last_movement;
    movement3d_average _movement3d_average;
};

#endif // ALGORITHM_GPU_HPP
