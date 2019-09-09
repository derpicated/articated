// algorithm_original.hpp

#ifndef ALGORITHM_ORIGINAL_HPP
#define ALGORITHM_ORIGINAL_HPP

#include <QMutex>
#include <QVideoFrame>

#include "../shared/operators.hpp"
#include "../vision_algorithm.hpp"
#include "movement3d/movement3d.hpp"
#include "movement3d/movement3d_filter.hpp"

class AlgorithmOriginal : public VisionAlgorithm {
    public:
    AlgorithmOriginal (QOpenGLContext& opengl_context, AugmentationWidget& augmentation);
    ~AlgorithmOriginal ();

    void SetReference () override;
    Movement3D Execute (const QVideoFrame& const_buffer) override;

    private:
    bool Process (image_t& image, Movement3D& movement);

    points_t markers_;
    points_t reference_;
    operators operators_;
    QMutex markers_mutex_;
    Movement3D last_movement_;
    Movement3DFilter movement3d_average_;
};

#endif // ALGORITHM_ORIGINAL_HPP
