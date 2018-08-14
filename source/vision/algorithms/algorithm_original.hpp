// algorithm_original.hpp

#ifndef ALGORITHM_ORIGINAL_HPP
#define ALGORITHM_ORIGINAL_HPP

#include <QMutex>
#include <QVideoFrame>

#include "movement3d/movement3d.hpp"
#include "movement3d/movement3d_filter.hpp"
#include "vision_algorithm.hpp"

class algorithm_original : public vision_algorithm {
    public:
    algorithm_original ();
    ~algorithm_original ();

    void set_reference () override;
    movement3d execute (const QVideoFrame& const_buffer) override;

    private:
    bool process (image_t& image, movement3d& movement);

    points_t _markers;
    points_t _reference;
    operators _operators;
    QMutex _markers_mutex;
    movement3d _last_movement;
    movement3d_average _movement3d_average;
};

#endif // ALGORITHM_ORIGINAL_HPP
