#include "video_sink_acquisition.hpp"

Q_LOGGING_CATEGORY (visionAqcuisitionSinkLog, "vision.acquisition.sink", QtInfoMsg)

void VideoSinkAcquisition::setSource (QCamera& camera) {
    // camera_->setViewfinder (&sink_);
}

void VideoSinkAcquisition::setSource (QMediaPlayer& player) {
    player.setVideoOutput (&sink_);
}

std::unique_ptr<AcquisitionInterface> createAcquisitionInstance () {
    return std::make_unique<VideoSinkAcquisition> ();
}
