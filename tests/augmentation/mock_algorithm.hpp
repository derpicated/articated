// mock_algorithm.hpp

#ifndef MOCK_ALGORITHM_HPP
#define MOCK_ALGORITHM_HPP

#include <QDebug>
#include <QObject>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLTexture>
#include <QQuickWindow>

#include "shared/frame_data.hpp"
#include "shared/movement3d/movement3d.hpp"

class MockAlgorithm : public QObject {
    Q_OBJECT
    Q_PROPERTY (float rotationX READ getRotationX WRITE setRotationX)
    Q_PROPERTY (float rotationY READ getRotationY WRITE setRotationY)
    Q_PROPERTY (float rotationZ READ getRotationZ WRITE setRotationZ)
    Q_PROPERTY (float translationX READ getTranslationX WRITE setTranslationX)
    Q_PROPERTY (float translationY READ getTranslationY WRITE setTranslationY)
    Q_PROPERTY (float scale READ getScale WRITE setScale)
    public:
    MockAlgorithm ();
    ~MockAlgorithm () = default;

    signals:
    void frameReady (FrameData frame);

    public slots:
    void loadTexture ();

    void setRotationX (const float value);
    float getRotationX () const;
    void setRotationY (const float value);
    float getRotationY () const;
    void setRotationZ (const float value);
    float getRotationZ () const;
    void setTranslationX (const float value);
    float getTranslationX () const;
    void setTranslationY (const float value);
    float getTranslationY () const;
    void setScale (const float value);
    float getScale () const;

    private:
    void composeFrame ();
    QOpenGLContext opengl_context_;
    QOffscreenSurface dummy_surface_;
    QOpenGLTexture* test_texture_{ nullptr };
    Movement3D transform_;
};

#endif // MOCK_ALGORITHM_HPP
