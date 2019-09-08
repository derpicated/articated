// algorithm_random.hpp

#ifndef ALGORITHM_RANDOM_HPP
#define ALGORITHM_RANDOM_HPP

#include <QMutex>
#include <QVideoFrame>

#include "vision_algorithm.hpp"

class AlgorithmRandom : public VisionAlgorithm {
    public:
    AlgorithmRandom (QOpenGLContext& opengl_context, AugmentationWidget& augmentation);
    ~AlgorithmRandom ();

    void SetReference ();
    movement3d Execute (const QVideoFrame& const_buffer);

    private:
    QMutex movement_mutex_;
    movement3d last_movement_;
    movement3d random_movement_;
};

#endif // ALGORITHM_RANDOM_HPP
