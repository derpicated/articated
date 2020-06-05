#include "augmentation_view.hpp"

#include "shared/movement3d/movement3d.hpp"

AugmentationView::AugmentationView ()
: m_renderer (nullptr) {
    connect (this, &QQuickItem::windowChanged, this, &AugmentationView::handleWindowChanged);
}

void AugmentationView::handleWindowChanged (QQuickWindow* win) {
    if (win) {
        connect (win, &QQuickWindow::beforeSynchronizing, this,
        &AugmentationView::sync, Qt::DirectConnection);
        connect (win, &QQuickWindow::sceneGraphInvalidated, this,
        &AugmentationView::cleanup, Qt::DirectConnection);
        // Ensure we start with cleared to black. The squircle's blend mode relies on this.
        win->setColor (Qt::black);
    }
}

void AugmentationView::drawFrame (int yaw) {
    Movement3D transform;
    transform.yaw (yaw);
    transform.scale (1);
    m_renderer->SetTransform (transform);
    if (window ()) {
        window ()->update ();
    }
}

void AugmentationView::LoadObject (const QString& path) {
    m_renderer->LoadObject (path);
}

void AugmentationView::sync () {
    if (!m_renderer) {
        m_renderer = new AugmentationRenderer ();
        connect (window (), &QQuickWindow::beforeRendering, m_renderer,
        &AugmentationRenderer::init, Qt::DirectConnection);
        connect (window (), &QQuickWindow::beforeRenderPassRecording,
        m_renderer, &AugmentationRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize (window ()->size () * window ()->devicePixelRatio ());
    m_renderer->setWindow (window ());
}

void AugmentationView::cleanup () {
    delete m_renderer;
    m_renderer = nullptr;
}

void AugmentationView::releaseResources () {
    window ()->scheduleRenderJob (
    new CleanupJob (m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}

// void AugmentationView::setrotation () {
//     if (window ()) window ()->update ();
