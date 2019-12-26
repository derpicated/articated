// augmentation_widget.cpp

#include "augmentation_widget.hpp"

#include <QOpenGLExtraFunctions>
#include <QTemporaryFile>
#include <QVector2D>
#include <QVector3D>
#include <math.h>

#ifdef ANDROID
#include <GLES3/gl3.h>
#endif

AugmentationWidget::AugmentationWidget (QWidget* parent)
: QOpenGLWidget (parent)
, transform_ ()
, opengl_mutex_ (QMutex::Recursive)
, is_grayscale_ (0)
, vertex_count_ (0) {
    Q_INIT_RESOURCE (GL_shaders);
}

AugmentationWidget::~AugmentationWidget () {
    opengl_mutex_.lock ();
    glDeleteBuffers (1, &object_vbo_);
    glDeleteVertexArrays (1, &object_vao_);
    opengl_mutex_.unlock ();
}

QSize AugmentationWidget::minimumSizeHint () const {
    return QSize (600, 350);
}

QSize AugmentationWidget::sizeHint () const {
    return QSize (600, 350);
}

bool AugmentationWidget::LoadObject (const QString& resource_path) {
    opengl_mutex_.lock ();
    makeCurrent ();
    bool status = false;

    // extract model from resources into filesystem and parse it
    QFile resource_file (resource_path);
    if (resource_file.exists ()) {
        auto temp_file  = QTemporaryFile::createNativeFile (resource_file);
        QString fs_path = temp_file->fileName ();

        if (!fs_path.isEmpty ()) {
            std::vector<float> model_interleaved = object_.Load (fs_path.toStdString ());

            glBindBuffer (GL_ARRAY_BUFFER, object_vbo_);
            glBufferData (GL_ARRAY_BUFFER, sizeof (float) * model_interleaved.size (),
            model_interleaved.data (), GL_STATIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, 0);
            vertex_count_ = model_interleaved.size () / object_.DataPerVertex ();
            object_.Unload ();

            status = true;
        }
    }
    doneCurrent ();
    opengl_mutex_.unlock ();
    return status;
}

void AugmentationWidget::DrawFrame (GLuint tex, bool is_grayscale, Movement3D transform) {
    SetBackground (tex, is_grayscale);
    SetTransform (transform);
    update ();
}

void AugmentationWidget::SetBackground (GLuint tex, bool is_grayscale) {
    opengl_mutex_.lock ();
    current_handle_ = tex;
    is_grayscale_   = is_grayscale;
    opengl_mutex_.unlock ();
}

GLuint AugmentationWidget::Background () {
    return texture_background_;
}

void AugmentationWidget::SetTransform (Movement3D transform) {
    opengl_mutex_.lock ();
    transform_ = transform;
    transform_.pitch (-(transform_.pitch () - 90));
    transform_.yaw (-transform_.yaw ());
    transform_.roll (-transform_.roll ());
    opengl_mutex_.unlock ();
}

Movement3D AugmentationWidget::Transform () {
    return transform_;
}

void AugmentationWidget::initializeGL () {
    opengl_mutex_.lock ();
    initializeOpenGLFunctions ();

    glClearColor (1, 0.5, 1, 1.0f);
    glEnable (GL_DEPTH_TEST);
    // glEnable (GL_CULL_FACE);

    glGenTextures (1, &texture_background_);
    glBindTexture (GL_TEXTURE_2D, texture_background_);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture (GL_TEXTURE_2D, 0);

    // generate a buffer to bind to textures
    glGenFramebuffers (1, &readback_buffer_);

    // compile and link shaders
    CompileShaders ();

    // generate vertex array buffers
    GenerateBuffers ();

    // setup projection matrix
    mat_projection_.ortho (-2.0f, +2.0f, -2.0f, +2.0f, 1.0f, 25.0f);

    emit InitializedOpenGL ();
    opengl_mutex_.unlock ();
}

void AugmentationWidget::GenerateBuffers () {
    opengl_mutex_.lock ();
    // setup background vao
    {
        glGenVertexArrays (1, &background_vao_);
        glGenBuffers (1, &background_vbo_);

        glBindVertexArray (background_vao_);
        glBindBuffer (GL_ARRAY_BUFFER, background_vbo_);

        int pos_location = program_background_.attributeLocation ("position");
        glVertexAttribPointer (pos_location, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof (float), reinterpret_cast<void*> (0));
        glEnableVertexAttribArray (pos_location);

        int tex_location = program_background_.attributeLocation ("tex");
        glVertexAttribPointer (tex_location, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof (float), reinterpret_cast<void*> (2 * sizeof (float)));
        glEnableVertexAttribArray (tex_location);

        // fill buffer with data
        GLfloat interleaved_background_buff[6 * 4] = {
            -1.0, 1.0,  // poly 1 a
            0.0, 0.0,   // poly 1 a tex
            -1.0, -1.0, // poly 1 b
            0.0, 1.0,   // poly 1 b tex
            1.0, 1.0,   // poly 1 c
            1.0, 0.0,   // poly 1 c tex
            1.0, 1.0,   // poly 2 a
            1.0, 0.0,   // poly 2 a tex
            -1.0, -1.0, // poly 2 b
            0.0, 1.0,   // poly 2 b tex
            1.0, -1.0,  // poly 2 c
            1.0, 1.0    // poly 2 c tex
        };
        glBufferData (GL_ARRAY_BUFFER, sizeof (float) * 6 * 4,
        interleaved_background_buff, GL_STATIC_DRAW);
    }

    // setup object vao
    {
        glGenVertexArrays (1, &object_vao_);
        glGenBuffers (1, &object_vbo_);

        glBindVertexArray (object_vao_);
        glBindBuffer (GL_ARRAY_BUFFER, object_vbo_);

        int pos_location = program_object_.attributeLocation ("position");
        glVertexAttribPointer (pos_location, 3, GL_FLOAT, GL_FALSE,
        10 * sizeof (float), reinterpret_cast<void*> (0));
        glEnableVertexAttribArray (pos_location);

        int nor_location = program_object_.attributeLocation ("normal");
        glVertexAttribPointer (nor_location, 3, GL_FLOAT, GL_FALSE,
        10 * sizeof (float), reinterpret_cast<void*> (3 * sizeof (float)));
        glEnableVertexAttribArray (nor_location);

        int col_location = program_object_.attributeLocation ("color");
        glVertexAttribPointer (col_location, 4, GL_FLOAT, GL_FALSE,
        10 * sizeof (float), reinterpret_cast<void*> (6 * sizeof (float)));
        glEnableVertexAttribArray (col_location);

        glBindBuffer (GL_ARRAY_BUFFER, 0);
        glBindVertexArray (0);
    }
    opengl_mutex_.unlock ();
}

void AugmentationWidget::CompileShaders () {
    opengl_mutex_.lock ();
    // background shaders
    {
        QFile vs_file (":/GL_shaders/background_vs.glsl");
        QFile fs_file (":/GL_shaders/background_fs.glsl");
        vs_file.open (QIODevice::ReadOnly);
        fs_file.open (QIODevice::ReadOnly);
        QByteArray vs_source = vs_file.readAll ();
        QByteArray fs_source = fs_file.readAll ();

        if (QOpenGLContext::currentContext ()->isOpenGLES ()) {
            vs_source.prepend (QByteArrayLiteral ("#version 300 es\n"));
            fs_source.prepend (QByteArrayLiteral ("#version 300 es\n"));
        } else {
            vs_source.prepend (QByteArrayLiteral ("#version 410\n"));
            fs_source.prepend (QByteArrayLiteral ("#version 410\n"));
        }

        program_background_.addShaderFromSourceCode (QOpenGLShader::Vertex, vs_source);
        program_background_.addShaderFromSourceCode (QOpenGLShader::Fragment, fs_source);
        program_background_.link ();

        program_background_.bind ();
        program_background_.setUniformValue ("u_tex_background", 0);
        program_background_.release ();
    }
    // object shaders
    {
        QFile vs_file (":/GL_shaders/object_vs.glsl");
        QFile fs_file (":/GL_shaders/object_fs.glsl");
        vs_file.open (QIODevice::ReadOnly);
        fs_file.open (QIODevice::ReadOnly);
        QByteArray vs_source = vs_file.readAll ();
        QByteArray fs_source = fs_file.readAll ();

        if (QOpenGLContext::currentContext ()->isOpenGLES ()) {
            vs_source.prepend (QByteArrayLiteral ("#version 300 es\n"));
            fs_source.prepend (QByteArrayLiteral ("#version 300 es\n"));
        } else {
            vs_source.prepend (QByteArrayLiteral ("#version 410\n"));
            fs_source.prepend (QByteArrayLiteral ("#version 410\n"));
        }

        program_object_.addShaderFromSourceCode (QOpenGLShader::Vertex, vs_source);
        program_object_.addShaderFromSourceCode (QOpenGLShader::Fragment, fs_source);
        program_object_.link ();
    }
    opengl_mutex_.unlock ();
}

void AugmentationWidget::resizeGL (int width, int height) {
    opengl_mutex_.lock ();
    view_width_  = width;
    view_height_ = height;

    mat_projection_.setToIdentity ();
    // TODO: replace with perspective, or possibly intrinsic camera matrix
    mat_projection_.ortho (-2.0f, +2.0f, -2.0f, +2.0f, 1.0f, 25.0f);
    opengl_mutex_.unlock ();
}

void AugmentationWidget::paintGL () {
    opengl_mutex_.lock ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport (0, 0, view_width_, view_height_);

    // draw background
    program_background_.bind ();
    program_background_.setUniformValue ("is_GLRED", is_grayscale_);
    DrawBackground ();

    // draw object
    QMatrix4x4 mat_modelview;
    mat_modelview.translate (
    transform_.translation ().x, transform_.translation ().y, -10.0);
    mat_modelview.scale (transform_.scale ());
    mat_modelview.rotate (transform_.pitch (), 1, 0, 0);
    mat_modelview.rotate (transform_.yaw (), 0, 1, 0);
    mat_modelview.rotate (transform_.roll (), 0, 0, 1);
    mat_modelview = mat_projection_ * mat_modelview;

    program_object_.bind ();
    program_object_.setUniformValue ("view_matrix", mat_modelview);
    DrawObject ();
    opengl_mutex_.unlock ();
}

void AugmentationWidget::DrawObject () {
    opengl_mutex_.lock ();
    // draw the object
    glBindVertexArray (object_vao_);
    glDrawArrays (GL_TRIANGLES, 0, vertex_count_);
    glBindVertexArray (0);
    opengl_mutex_.unlock ();
}

void AugmentationWidget::DrawBackground () {
    opengl_mutex_.lock ();
    // draw the 2 triangles that form the background
    glBindVertexArray (background_vao_);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, current_handle_);
    glDrawArrays (GL_TRIANGLES, 0, 6);
    glBindTexture (GL_TEXTURE_2D, 0);
    glBindVertexArray (0);
    opengl_mutex_.unlock ();
}
