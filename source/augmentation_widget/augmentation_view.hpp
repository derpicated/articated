// augmentation_view.hpp

#ifndef AUGMENTATION_VIEW_HPP
#define AUGMENTATION_VIEW_HPP

#include <QRunnable>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>

#include "augmentation_renderer.hpp"
#include "shared/frame_data.hpp"

class CleanupJob : public QRunnable {
    public:
    CleanupJob (AugmentationRenderer* renderer)
    : renderer_ (renderer) {
    }
    void run () override {
        delete renderer_;
    }

    private:
    AugmentationRenderer* renderer_;
};

class AugmentationView : public QQuickItem {
    Q_OBJECT
    public:
    AugmentationView ();

    signals:

    public slots:
    void sync ();
    void cleanup ();
    void drawFrame (FrameData frame);
    void LoadObject (const QString& path);

    private slots:
    void handleWindowChanged (QQuickWindow* win);

    private:
    void releaseResources () override;

    AugmentationRenderer* renderer_;
    QString object_path_;
    Movement3D transform_;
};

#endif // AUGMENTATION_VIEW_HPP
