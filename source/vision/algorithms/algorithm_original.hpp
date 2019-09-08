// algorithm_original.hpp

#ifndef ALGORITHM_ORIGINAL_HPP
#define ALGORITHM_ORIGINAL_HPP

#include <QMutex>
#include <QVideoFrame>

#include "movement3d/movement3d.hpp"
#include "movement3d/movement3d_filter.hpp"
#include "vision_algorithm.hpp"

class AlgorithmOriginal : public VisionAlgorithm {
    public:
    AlgorithmOriginal (QOpenGLContext& opengl_context, AugmentationWidget& augmentation);
    ~AlgorithmOriginal ();

    void SetReference () override;
    movement3d Execute (const QVideoFrame& const_buffer) override;

    private:
    bool Process (image_t& image, movement3d& movement);

    points_t markers_;
    points_t reference_;
    operators operators_;
    QMutex markers_mutex_;
    movement3d last_movement_;
    movement3d_average movement3d_average_;
};

#endif // ALGORITHM_ORIGINAL_HPP
