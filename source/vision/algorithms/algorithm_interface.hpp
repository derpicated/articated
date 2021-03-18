// vision_algorithm.hpp

#ifndef VISION_ALGORITHM_HPP
#define VISION_ALGORITHM_HPP

#include <QtMultimedia/QVideoFrame>

#include "shared/frame_data.hpp"

class AlgorithmInterface {
    public:
    virtual ~AlgorithmInterface () = default;

    [[nodiscard]] virtual int MaxDebugLevel () const  = 0;
    [[nodiscard]] virtual int DebugLevel () const     = 0;
    virtual void SetDebugLevel (const int& new_level) = 0;

    virtual void SetReference ()                                = 0;
    virtual FrameData Execute (const QVideoFrame& const_buffer) = 0;
};

#endif // VISION_ALGORITHM_HPP
