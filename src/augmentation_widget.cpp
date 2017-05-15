// augmentation_widget.cpp

#include "augmentation_widget.hpp"

#include <QOpenGLExtraFunctions>
#include <QTemporaryFile>
#include <QVector2D>
#include <QVector3D>
#include <math.h>

augmentation_widget::augmentation_widget (QWidget* parent)
: QOpenGLWidget (parent)
, _scale_factor (1.0f)
, _x_pos (0.0f)
, _y_pos (0.0f)
, _vertex_count (0) {
}

augmentation_widget::~augmentation_widget () {
    glDeleteBuffers (1, &_object_vbo);
    glDeleteVertexArrays (1, &_object_vao);
}

QSize augmentation_widget::minimumSizeHint () const {
    return QSize (600, 350);
}

QSize augmentation_widget::sizeHint () const {
    return QSize (600, 350);
}

bool augmentation_widget::loadObject (const QString& resource_path) {
    bool status = false;

    // extract model from resources into filesystem and parse it
    QFile resource_file (resource_path);
    if (resource_file.exists ()) {
        auto temp_file  = QTemporaryFile::createNativeFile (resource_file);
        QString fs_path = temp_file->fileName ();

        if (!fs_path.isEmpty ()) {
            std::vector<float> model_interleaved = _object.load (fs_path.toStdString ());

            glBindBuffer (GL_ARRAY_BUFFER, _object_vbo);
            glBufferData (GL_ARRAY_BUFFER, sizeof (float) * model_interleaved.size (),
            model_interleaved.data (), GL_STATIC_DRAW);
            glBindBuffer (GL_ARRAY_BUFFER, 0);
            _vertex_count = model_interleaved.size () / _object.data_per_vertex ();
            _object.release ();

            status = true;
        }
    }
    update ();
    return status;
}

void augmentation_widget::setBackground (image_t image) {
    // create background texture
    glBindTexture (GL_TEXTURE_2D, _texture_background);

    GLint format_gl;

    switch (image.format) {
        case RGB24: {
            format_gl = GL_RGB;
            break;
        }
        case YUV:
        case GREY8:
        case BINARY8: {
            format_gl = GL_LUMINANCE;
            break;
        }
    }
    glTexImage2D (GL_TEXTURE_2D, 0, format_gl, image.width, image.height, 0,
    format_gl, GL_UNSIGNED_BYTE, image.data);

    // normalize coordinates
    // TODO: if needed, might be redundant if self written shader already
    // normalizes texture coords


    /*    glMatrixMode (GL_TEXTURE);
        glLoadIdentity ();
        glScalef (1.0 / image.width, 1.0 / image.height, 1);
        glMatrixMode (GL_MODELVIEW);*/
}

void augmentation_widget::setScale (const float scale) {
    _scale_factor = scale;
}

void augmentation_widget::setXPosition (const float location) {
    _x_pos = location;
}

void augmentation_widget::setYPosition (const float location) {
    _y_pos = location;
}

void augmentation_widget::setXRotation (const GLfloat angle) {
    _x_rot = angle;
}

void augmentation_widget::setYRotation (const GLfloat angle) {
    _y_rot = angle;
}

void augmentation_widget::setZRotation (const GLfloat angle) {
    _z_rot = angle;
}

void augmentation_widget::initializeGL () {
    int status = 0;
    initializeOpenGLFunctions ();

    glClearColor (1, 0.5, 1, 1.0f);
    glEnable (GL_DEPTH_TEST);
    // glEnable (GL_CULL_FACE);

    // TODO: add lighting back
    /*    glShadeModel (GL_SMOOTH);
        glEnable (GL_LIGHTING);
        glEnable (GL_LIGHT0);
        glEnable (GL_COLOR_MATERIAL);
            glMatrixMode (GL_PROJECTION);*/
    glEnable (GL_TEXTURE_2D);

    glGenTextures (1, &_texture_background);
    glBindTexture (GL_TEXTURE_2D, _texture_background);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);


    // compile and link shaders
    compile_shaders ();

    // generate vertex array buffers
    generate_buffers ();

    // TODO: add lighting back
    /*glMatrixMode (GL_MODELVIEW);
    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
    mat_identity ();
    // gluPerspective (33.7, 1.3, 0.1, 100.0);*/
    _mat_projection.ortho (-2.0f, +2.0f, -2.0f, +2.0f, 1.0f, 25.0f);

    emit initialized ();
}

void augmentation_widget::generate_buffers () {
    // setup background vao
    {
        glGenVertexArrays (1, &_background_vao);
        glGenBuffers (1, &_background_vbo);

        glBindVertexArray (_background_vao);
        glBindBuffer (GL_ARRAY_BUFFER, _background_vbo);

        int pos_location = _program_background.attributeLocation ("position");
        glVertexAttribPointer (pos_location, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof (float), reinterpret_cast<void*> (0));
        glEnableVertexAttribArray (pos_location);

        int tex_location = _program_background.attributeLocation ("tex");
        glVertexAttribPointer (tex_location, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof (float), reinterpret_cast<void*> (2 * sizeof (float)));
        glEnableVertexAttribArray (tex_location);

        // fill buffer with data
        GLfloat interleaved_background_buff[6 * 4] = {
            -1.0, 1.0,  // poly 1 a
            0.0, 1.0,   // poly 1 a tex
            -1.0, -1.0, // poly 1 b
            0.0, 0.0,   // poly 1 b tex
            1.0, 1.0,   // poly 1 c
            1.0, 1.0,   // poly 1 c tex
            1.0, 1.0,   // poly 2 a
            1.0, 1.0,   // poly 2 a tex
            -1.0, -1.0, // poly 2 b
            0.0, 0.0,   // poly 2 b tex
            1.0, -1.0,  // poly 2 c
            1.0, 0.0    // poly 2 c tex
        };
        glBufferData (GL_ARRAY_BUFFER, sizeof (float) * 6 * 4,
        interleaved_background_buff, GL_STATIC_DRAW);

        // bind texture
        int tex_uniform =
        _program_background.uniformLocation ("u_tex_background");
        glActiveTexture (GL_TEXTURE0);
        glBindTexture (GL_TEXTURE_2D, _texture_background);
        glUniform1i (tex_uniform, 0);
    }

    // setup object vao
    {
        glGenVertexArrays (1, &_object_vao);
        glGenBuffers (1, &_object_vbo);

        glBindVertexArray (_object_vao);
        glBindBuffer (GL_ARRAY_BUFFER, _object_vbo);

        int pos_location = _program_object.attributeLocation ("position");
        glVertexAttribPointer (pos_location, 3, GL_FLOAT, GL_FALSE,
        10 * sizeof (float), reinterpret_cast<void*> (0));
        glEnableVertexAttribArray (pos_location);

        int nor_location = _program_object.attributeLocation ("normal");
        glVertexAttribPointer (nor_location, 3, GL_FLOAT, GL_FALSE,
        10 * sizeof (float), reinterpret_cast<void*> (3 * sizeof (float)));
        glEnableVertexAttribArray (nor_location);

        int col_location = _program_object.attributeLocation ("color");
        glVertexAttribPointer (col_location, 4, GL_FLOAT, GL_FALSE,
        10 * sizeof (float), reinterpret_cast<void*> (6 * sizeof (float)));
        glEnableVertexAttribArray (col_location);

        glBindBuffer (GL_ARRAY_BUFFER, 0);
        glBindVertexArray (0);
    }
}

void augmentation_widget::compile_shaders () {
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

        _program_background.addShaderFromSourceCode (QOpenGLShader::Vertex, vs_source);
        _program_background.addShaderFromSourceCode (QOpenGLShader::Fragment, fs_source);
        _program_background.link ();
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

        _program_object.addShaderFromSourceCode (QOpenGLShader::Vertex, vs_source);
        _program_object.addShaderFromSourceCode (QOpenGLShader::Fragment, fs_source);
        _program_object.link ();
    }
}

void augmentation_widget::resizeGL (int width, int height) {
    int side = qMin (width, height);
    glViewport ((width - side) / 2, (height - side) / 2, side, side);

    _mat_projection.setToIdentity ();
    // TODO: replace with perspective
    _mat_projection.ortho (-2.0f, +2.0f, -2.0f, +2.0f, 1.0f, 25.0f);
}

void augmentation_widget::paintGL () {
    // QOpenGLFunctions* f = QOpenGLContext::currentContext
    // ()->functions
    // ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // draw background
    _program_background.bind ();
    draw_background ();
    // glClear (GL_DEPTH_BUFFER_BIT);

    _program_object.bind ();
    // TODO: why is pushing/duplicating the matrix needed?
    // glPushMatrix ();

    // draw object
    // TODO: findout if this is the correct order, should translation not
    // happen after rotation?
    QMatrix4x4 mat_modelview, mat_normal;
    mat_modelview.translate (_x_pos, _y_pos, -10.0);
    mat_modelview.scale (_scale_factor);
    mat_modelview.rotate (_x_rot, 1, 0, 0);
    mat_modelview.rotate (_y_rot, 0, 1, 0);
    mat_modelview.rotate (_z_rot, 0, 0, 1);
    mat_modelview = _mat_projection * mat_modelview;

    mat_normal.rotate (_x_rot, 1, 0, 0);
    mat_normal.rotate (_y_rot, 0, 1, 0);
    mat_normal.rotate (_z_rot, 0, 0, 1);

    _program_object.setUniformValue ("normal_matrix", mat_normal);
    _program_object.setUniformValue ("view_matrix", mat_modelview);

    draw_object ();

    // TODO: also see todo at glPushMatrix, is pushing and popping needed
    // here? glPopMatrix ();
}

void augmentation_widget::draw_object () {
    // draw the object
    glBindVertexArray (_object_vao);
    glDrawArrays (GL_TRIANGLES, 0, _vertex_count);
    glBindVertexArray (0);
}

void augmentation_widget::draw_background () {
    // draw the 2 triangles that form the background
    glBindVertexArray (_background_vao);
    glBindTexture (GL_TEXTURE_2D, _texture_background);
    glDrawArrays (GL_TRIANGLES, 0, 6);
    glBindVertexArray (0);
}
