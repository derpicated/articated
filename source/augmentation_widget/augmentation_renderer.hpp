// augmentation_renderer.hpp

#ifndef AUGMENTATION_RENDERER_HPP
#define AUGMENTATION_RENDERER_HPP

#include <QMatrix4x4>
#include <QMutex>
#include <QObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QtQuick/QQuickWindow>

#include "model_loader.hpp"
#include "shared/framedata.hpp"
#include "shared/movement3d/movement3d.hpp"

class AugmentationRenderer : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
    public:
    AugmentationRenderer (QObject* parent = 0);
    ~AugmentationRenderer ();

    // QOpenGLWidget reimplemented functions
    void init ();
    void setViewportSize (const QSize& size);
    void setWindow (QQuickWindow* window);
    void paint ();

    public slots:
    void DrawFrame (FrameData frame_data);
    void SetObject (const QString& path);
    void SetBackground (GLuint tex, bool is_grayscale);
    GLuint Background ();
    void SetTransform (Movement3D transform);
    Movement3D Transform ();

    signals:
    void InitializedOpenGL ();

    private:
    bool LoadObject (const QString& path);
    void GenerateBuffers ();
    void CompileShaders ();
    void DrawObject ();
    void DrawBackground ();

    bool is_initialized_{ false };
    ModelLoader object_;
    QString object_path_;
    QQuickWindow* window_;
    int view_width_;
    int view_height_;
    Movement3D transform_;
    QMutex opengl_mutex_;
    QMatrix4x4 mat_projection_;
    GLuint is_grayscale_;
    GLuint texture_background_;
    GLuint current_handle_;
    GLuint readback_buffer_;
    GLuint background_vao_;
    GLuint object_vao_;
    GLuint background_vbo_;
    GLuint object_vbo_;
    GLuint vertex_count_;
    QOpenGLShaderProgram program_background_;
    QOpenGLShaderProgram program_object_;
};

#endif // AUGMENTATION_RENDERER_HPP
