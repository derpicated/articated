// algorithm_random.hpp

#ifndef ALGORITHM_RANDOM_HPP
#define ALGORITHM_RANDOM_HPP

#include <QLoggingCategory>
#include <QMutex>
#include <QVideoFrame>

#include "algorithm_interface.hpp"
#include "frame_helper_interface.hpp"
#include "shared/movement3d/movement3d.hpp"

Q_DECLARE_LOGGING_CATEGORY (visionAlgorithmRandomLog)

class AlgorithmRandom final : public AlgorithmInterface {
    public:
    AlgorithmRandom ();
    ~AlgorithmRandom () final = default;

    [[nodiscard]] int MaxDebugLevel () const final;
    [[nodiscard]] int DebugLevel () const final;
    void SetDebugLevel (const int& new_level) final;

    void SetReference () final;
    FrameData Execute (const QVideoFrame& const_buffer) final;

    private:
    std::unique_ptr<FrameHelperInterface> frame_helper_;
    const int max_debug_level_;
    int debug_level_{};
    QMutex movement_mutex_;
    Movement3D last_movement_;
    Movement3D random_movement_;
};

#endif // ALGORITHM_RANDOM_HPP
