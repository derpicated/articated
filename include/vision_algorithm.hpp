// vision_algorithm.hpp

#ifndef VISION_ALGORITHM_HPP
#define VISION_ALGORITHM_HPP

#include <QVideoFrame>

#include "augmentation_widget.hpp"
#include "movement3d.hpp"
#include "operators.hpp"

class vision_algorithm {
    public:
    vision_algorithm (augmentation_widget& augmentation, const int& max_debug_level);
    virtual ~vision_algorithm (){};

    int max_debug_level ();
    void set_debug_level (const int& new_level);
    int debug_level ();

    virtual void set_reference ()                                = 0;
    virtual movement3d execute (const QVideoFrame& const_buffer) = 0;

    protected:
    bool frame_to_ram (const QVideoFrame& const_buffer, image_t& image);

    augmentation_widget& _augmentation;
    const int _max_debug_level;
    int _debug_level;
};

#endif // VISION_ALGORITHM_HPP
