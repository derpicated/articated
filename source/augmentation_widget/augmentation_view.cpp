#include "augmentation_view.hpp"

#include <QDir>
#include <iostream>

#include "shared/movement3d/movement3d.hpp"

AugmentationView::AugmentationView ()
: renderer_ (nullptr) {
    Q_INIT_RESOURCE (3D_models);
    readModels ();
    setModel (0);
    connect (this, &QQuickItem::windowChanged, this, &AugmentationView::handleWindowChanged);
}

AugmentationView::~AugmentationView () {
    Q_CLEANUP_RESOURCE (3D_models);
}

void AugmentationView::readModels () {
    models_ = QDir (":/3D_models/").entryList (QDir::Files);
    emit modelsChanged ();
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

    try {
        transform_          = std::any_cast<Movement3D> (data["transform"]);
        background_texture_ = std::any_cast<GLuint> (data["background"]);

        if (auto is_grayscale_it = data.find ("backgroundIsGrayscale");
            is_grayscale_it != data.end ()) {
            background_is_grayscale_ = std::any_cast<bool> (is_grayscale_it->second);
        } else {
            background_is_grayscale_ = false;
        }
    } catch (const std::bad_any_cast& e) {
        std::cout << e.what () << '\n';
    }

    if (window ()) {
        window ()->update ();
    }
}

void AugmentationView::setModel (int model) {
    model_       = model;
    object_path_ = models_[model];
    emit modelChanged ();
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
    renderer_->SetBackground (background_texture_, background_is_grayscale_);
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
