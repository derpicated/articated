#include "augmentation_view.hpp"

#include "shared/movement3d/movement3d.hpp"
#include <iostream>

AugmentationView::AugmentationView ()
: renderer_ (nullptr) {
    connect (this, &QQuickItem::windowChanged, this, &AugmentationView::handleWindowChanged);
}

void AugmentationView::handleWindowChanged (QQuickWindow* win) {
    if (win) {
        connect (win, &QQuickWindow::beforeSynchronizing, this,
        &AugmentationView::sync, Qt::DirectConnection);
        connect (win, &QQuickWindow::sceneGraphInvalidated, this,
        &AugmentationView::cleanup, Qt::DirectConnection);
        win->setColor ("magenta");
    }
}

void AugmentationView::drawFrame (FrameData frame) {
    auto& data = frame.data;
    transform_ = std::any_cast<Movement3D> (data["transform"]);

    if (window ()) {
        window ()->update ();
    }
}

void AugmentationView::LoadObject (const QString& path) {
    object_path_ = path;
}

void AugmentationView::sync () {
    if (!renderer_) {
        renderer_ = new AugmentationRenderer ();
        connect (window (), &QQuickWindow::beforeRendering, renderer_,
        &AugmentationRenderer::init, Qt::DirectConnection);
        connect (window (), &QQuickWindow::beforeRenderPassRecording, renderer_,
        &AugmentationRenderer::paint, Qt::DirectConnection);
    }

    renderer_->setViewportSize (window ()->size () * window ()->devicePixelRatio ());
    renderer_->setWindow (window ());
    renderer_->SetTransform (transform_);
    renderer_->SetObject (object_path_);
}

void AugmentationView::cleanup () {
    delete renderer_;
    renderer_ = nullptr;
}

void AugmentationView::releaseResources () {
    window ()->scheduleRenderJob (
    new CleanupJob (renderer_), QQuickWindow::BeforeSynchronizingStage);
    renderer_ = nullptr;
}
