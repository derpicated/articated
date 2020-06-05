// augmentation_view.hpp

#ifndef AUGMENTATION_VIEW_HPP
#define AUGMENTATION_VIEW_HPP

#include <QRunnable>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>

#include "augmentation_renderer.hpp"

class CleanupJob : public QRunnable {
    public:
    CleanupJob (AugmentationRenderer* renderer)
    : m_renderer (renderer) {
    }
    void run () override {
        delete m_renderer;
    }

    private:
    AugmentationRenderer* m_renderer;
};

class AugmentationView : public QQuickItem {
    Q_OBJECT
    // Q_PROPERTY (qreal t READ t WRITE setT NOTIFY tChanged)

    public:
    AugmentationView ();
#warning remove references to T
    // qreal t () const {
    //     return m_t;
    // }
    // void setT (qreal t);

    signals:
    // void tChanged ();

    public slots:
    void sync ();
    void cleanup ();
    void drawFrame (int yaw);
    void LoadObject (const QString& path);

    private slots:
    void handleWindowChanged (QQuickWindow* win);

    private:
    void releaseResources () override;

    // qreal m_t;
    AugmentationRenderer* m_renderer;
};

#endif // AUGMENTATION_WIDGET_HPP
