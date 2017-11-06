// algorithm_random.hpp

#ifndef ALGORITHM_RANDOM_HPP
#define ALGORITHM_RANDOM_HPP

#include <QMutex>
#include <QVideoFrame>

#include "vision_algorithm.hpp"

class algorithm_random : public vision_algorithm {
    public:
    algorithm_random (augmentation_widget& augmentation);
    ~algorithm_random ();

    void set_reference ();
    movement3d execute (const QVideoFrame& const_buffer);

    private:
    QMutex _movement_mutex;
    movement3d _last_movement;
    movement3d _random_movement;
};

#endif // ALGORITHM_RANDOM_HPP
