// algorithm_original.hpp

#ifndef ALGORITHM_ORIGINAL_HPP
#define ALGORITHM_ORIGINAL_HPP

#include <QLoggingCategory>
#include <QMutex>
#include <QVideoFrame>

#include "../algorithm_interface.hpp"
#include "../utils/frame_helper.hpp"
#include "../utils/operators.hpp"
#include "shared/movement3d/movement3d.hpp"
#include "shared/movement3d/movement3d_filter.hpp"

Q_DECLARE_LOGGING_CATEGORY (visionAlgorithmOriginalLog)

class AlgorithmOriginal final : public AlgorithmInterface {
    public:
    ~AlgorithmOriginal () final = default;

    [[nodiscard]] int MaxDebugLevel () const final;
    [[nodiscard]] int DebugLevel () const final;
    void SetDebugLevel (const int& new_level) final;

    void SetReference () final;
    FrameData Execute (const QVideoFrame& const_buffer) final;

    private:
    bool Process (image_t& image, Movement3D& movement);
    void SetBackground (image_t image);

    FrameHelper frame_helper_;
    const int max_debug_level_{ 3 };
    int debug_level_{ 0 };
    bool background_is_grayscale_{};
    GLuint background_tex_{};
    points_t markers_;
    points_t reference_;
    operators operators_;
    QMutex markers_mutex_;
    Movement3D last_movement_;
    Movement3DFilter movement3d_average_{ 1 };
};

#endif // ALGORITHM_ORIGINAL_HPP
