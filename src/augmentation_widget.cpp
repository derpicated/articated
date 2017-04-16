// augmentation_widget.cpp

#include "augmentation_widget.hpp"

#include <QOpenGLExtraFunctions>
#include <QTemporaryFile>
#include <QVector2D>
#include <QVector3D>
#include <math.h>

// clang-format off
#define MATRIX_INITVAL  1, 0, 0, 0,\
                        0, 1, 0, 0,\
                        0, 0, 1, 0,\
                        0, 0, 0, 1
// clang-format on

augmentation_widget::augmentation_widget (QWidget* parent)
: QOpenGLWidget (parent)
, _scale_factor (1.0f)
, _x_pos (0.0f)
, _y_pos (0.0f)
, _mat_x_rot{ MATRIX_INITVAL }
, _mat_y_rot{ MATRIX_INITVAL }
, _mat_z_rot{ MATRIX_INITVAL }
, _mat_projection{ MATRIX_INITVAL } {
}

augmentation_widget::~augmentation_widget () {
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
            std::vector<float> model_interleafed = _object.load (fs_path.toStdString ());
            status = upload_to_gpu (model_interleafed);
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
#ifdef OPENGL_ES
            format_gl = GL_LUMINANCE;
#else
            format_gl = GL_RED;
#endif // OPENGL_ES
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

void augmentation_widget::setXRotation (const GLfloat persp_mat[16]) {
    for (int i = 0; i < 16; i++) {
        _mat_x_rot[i] = persp_mat[i];
    }
}

void augmentation_widget::setYRotation (const GLfloat persp_mat[16]) {
    for (int i = 0; i < 16; i++) {
        _mat_y_rot[i] = persp_mat[i];
    }
}

void augmentation_widget::setZRotation (const GLfloat persp_mat[16]) {
    for (int i = 0; i < 16; i++) {
        _mat_z_rot[i] = persp_mat[i];
    }
}

void augmentation_widget::setXRotation (const GLfloat angle) {
    mat_from_angle (_mat_x_rot, angle, 1, 0, 0);
}

void augmentation_widget::setYRotation (const GLfloat angle) {
    mat_from_angle (_mat_y_rot, angle, 0, 1, 0);
}

void augmentation_widget::setZRotation (const GLfloat angle) {
    mat_from_angle (_mat_z_rot, angle, 0, 0, 1);
}

void augmentation_widget::mat_from_angle (GLfloat mat[16], GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    float deg2rad = 3.14159265f / 180.0f;
    float c       = cosf (angle * deg2rad);
    float s       = sinf (angle * deg2rad);
    float c1      = 1.0f - c;

    // build rotation matrix
    mat[0]  = (x * x * c1) + c;
    mat[1]  = (x * y * c1) - (z * s);
    mat[2]  = (x * z * c1) + (y * s);
    mat[3]  = 0;
    mat[4]  = (y * x * c1) + (z * s);
    mat[5]  = (y * y * c1) + c;
    mat[6]  = (y * z * c1) - (x * s);
    mat[7]  = 0;
    mat[8]  = (z * x * c1) - (y * s);
    mat[9]  = (z * y * c1) + (x * s);
    mat[10] = (z * z * c1) + c;
    mat[11] = 0;
    mat[12] = 0;
    mat[13] = 0;
    mat[14] = 0;
    mat[15] = 1;
}

void augmentation_widget::mat_multiply (GLfloat a[16], const GLfloat b[16]) {
    // multiply matices a * b and store into a
    // c is used as a temporary copy of a
    // the matrices are column major
    GLfloat c[16];
    c[0]  = a[0];
    c[1]  = a[1];
    c[2]  = a[2];
    c[3]  = a[3];
    c[4]  = a[4];
    c[5]  = a[5];
    c[6]  = a[6];
    c[7]  = a[7];
    c[8]  = a[8];
    c[9]  = a[9];
    c[10] = a[10];
    c[11] = a[11];
    c[12] = a[12];
    c[13] = a[13];
    c[14] = a[14];
    c[15] = a[15];

    // first column
    a[0] = c[0] * b[0] + c[4] * b[1] + c[8] * b[2] + c[12] * b[3];
    a[1] = c[1] * b[0] + c[5] * b[1] + c[9] * b[2] + c[13] * b[3];
    a[2] = c[2] * b[0] + c[6] * b[1] + c[10] * b[2] + c[14] * b[3];
    a[3] = c[3] * b[0] + c[7] * b[1] + c[11] * b[2] + c[15] * b[3];
    // second column
    a[4] = c[0] * b[4] + c[4] * b[5] + c[8] * b[6] + c[12] * b[7];
    a[5] = c[1] * b[4] + c[5] * b[5] + c[9] * b[6] + c[13] * b[7];
    a[6] = c[2] * b[4] + c[6] * b[5] + c[10] * b[6] + c[14] * b[7];
    a[7] = c[3] * b[4] + c[7] * b[5] + c[11] * b[6] + c[15] * b[7];
    // third column
    a[8]  = c[0] * b[8] + c[4] * b[9] + c[8] * b[10] + c[12] * b[11];
    a[9]  = c[1] * b[8] + c[5] * b[9] + c[9] * b[10] + c[13] * b[11];
    a[10] = c[2] * b[8] + c[6] * b[9] + c[10] * b[10] + c[14] * b[11];
    a[11] = c[3] * b[8] + c[7] * b[9] + c[11] * b[10] + c[15] * b[11];
    // fourth column
    a[12] = c[0] * b[12] + c[4] * b[13] + c[8] * b[14] + c[12] * b[15];
    a[13] = c[1] * b[12] + c[5] * b[13] + c[9] * b[14] + c[13] * b[15];
    a[14] = c[2] * b[12] + c[6] * b[13] + c[10] * b[14] + c[14] * b[15];
    a[15] = c[3] * b[12] + c[7] * b[13] + c[11] * b[14] + c[15] * b[15];
}

void augmentation_widget::mat_scale (GLfloat mat_a[16], GLfloat x, GLfloat y, GLfloat z) {
    GLfloat mat_b[16] = { 0 };
    mat_b[0]          = x;
    mat_b[5]          = y;
    mat_b[10]         = z;
    mat_b[15]         = 1;

    mat_multiply (mat_a, mat_b);
}

void augmentation_widget::mat_translate (GLfloat mat_a[16], GLfloat x, GLfloat y, GLfloat z) {
    GLfloat mat_b[16] = { 0 };
    mat_b[0]          = 1;
    mat_b[5]          = 1;
    mat_b[10]         = 1;
    mat_b[12]         = x;
    mat_b[13]         = y;
    mat_b[14]         = z;
    mat_b[15]         = 1;

    mat_multiply (mat_a, mat_b);
}

void augmentation_widget::mat_ortho (GLfloat mat_a[16],
GLfloat left,
GLfloat right,
GLfloat bottom,
GLfloat top,
GLfloat near,
GLfloat far) {
    GLfloat mat_b[16] = { 0 };
    mat_b[0]          = 2 / (right - left);
    mat_b[5]          = 2 / (top - bottom);
    mat_b[10]         = -2 / (far - near);
    mat_b[12]         = -((right + left) / (right - left));
    mat_b[13]         = -((top + bottom) / (top - bottom));
    mat_b[14]         = -((far + near) / (far - near));
    mat_b[15]         = 1;

    mat_multiply (mat_a, mat_b);
}

void augmentation_widget::mat_identity (GLfloat mat[16]) {
    memset (mat, 0, sizeof (GLfloat) * 16);
    mat[0]  = 1;
    mat[5]  = 1;
    mat[10] = 1;
    mat[15] = 1;
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

    _program.addShaderFromSourceFile (QOpenGLShader::Vertex, ":/GL_shaders/basic_vs.glsl");
    _program.addShaderFromSourceFile (QOpenGLShader::Fragment, ":/GL_shaders/basic_fs.glsl");
    _program.link ();
    _program.bind ();

    // TODO: add lighting back
    /*glMatrixMode (GL_MODELVIEW);
    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv (GL_LIGHT0, GL_POSITION, lightPosition);
    mat_identity ();
    // gluPerspective (33.7, 1.3, 0.1, 100.0);*/
}

void augmentation_widget::resizeGL (int width, int height) {
    int side = qMin (width, height);
    glViewport ((width - side) / 2, (height - side) / 2, side, side);

    mat_identity (_mat_projection);
    mat_ortho (_mat_projection, -2.0f, +2.0f, -2.0f, +2.0f, 1.0f, 25.0f);
}

void augmentation_widget::paintGL () {
    GLfloat mat_modelview[16] = { MATRIX_INITVAL };
    // QOpenGLFunctions* f = QOpenGLContext::currentContext
    // ()->functions
    // ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // draw background
    mat_translate (mat_modelview, 0.0, 0.0, -10.0);
    // draw_background ();

    // TODO: why is pushing/duplicating the matrix needed?
    // glPushMatrix ();

    // draw object
    // TODO: findout if this is the correct order, should translation not happen
    // after rotation?
    mat_translate (mat_modelview, _x_pos, _y_pos, 0);
    mat_scale (mat_modelview, _scale_factor, _scale_factor, _scale_factor);
    mat_multiply (mat_modelview, _mat_x_rot);
    mat_multiply (mat_modelview, _mat_y_rot);
    mat_multiply (mat_modelview, _mat_z_rot);

    // TODO: draw object
    //_object.draw ();

    // TODO: also see todo at glPushMatrix, is pushing and popping needed here?
    // glPopMatrix ();
}

void augmentation_widget::draw_background () {
    /*
        glBindTexture (GL_TEXTURE_2D, _texture_background);
        glBegin (GL_QUADS);
        glColor3f (1, 1, 1);
        glTexCoord2f (0.0, 1.0);
        glVertex3f (-2.0, -2.0, -2.0);
        glTexCoord2f (1.0, 1.0);
        glVertex3f (2.0, -2.0, -2.0);
        glTexCoord2f (1.0, 0.0);
        glVertex3f (2.0, 2.0, -2.0);
        glTexCoord2f (0.0, 0.0);
        glVertex3f (-2.0, 2.0, -2.0);
        glEnd ();*/
    // clang-format off
    GLfloat vertices_buff[6 * 3] = {    -4.0, -3.0, -2.0,   // poly 1 a
                                        4.0, -3.0, -2.0,    // poly 1 b
                                        4.0, 3.0, -2.0,     // poly 1 c
                                        -4.0, -3.0, -.0,   // poly 2 a
                                        4.0, 3.0, -0.0,     // poly 2 b
                                        -4.0, 3.0, -0.0 };  // poly 2 c

    GLfloat normals_buff[6 * 3] = {     0, 0, 1,           // poly 1 a
                                        0, 0, 1,           // poly 1 b
                                        0, 0, 1,           // poly 1 c
                                        0, 0, 1,           // poly 2 a
                                        0, 0, 1,           // poly 2 b
                                        0, 0, 1 };         // poly 2 c

    GLfloat texture_buff[6 * 2] = {     0.0, 1.0,           // poly 1 a
                                        1.0, 1.0,           // poly 1 b
                                        1.0, 0.0,           // poly 1 c
                                        0.0, 1.0,           // poly 2 a
                                        1.0, 0.0,           // poly 2 b
                                        0.0, 0.0 };         // poly 2 c

    GLfloat colors_buff[6 * 4];
    // clang-format on
    // all collors white
    for (int i = 0; i < (6 * 4); i++) {
        colors_buff[i] = 1.0f;
    }

    /*glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);
    glEnableClientState (GL_COLOR_ARRAY);
    // glEnableClientState (GL_TEXTURE_COORD_ARRAY);

    glBindTexture (GL_TEXTURE_2D, _texture_background);
    glVertexPointer (3, GL_FLOAT, 0, vertices_buff);
    glNormalPointer (GL_FLOAT, 0, normals_buff);
    glColorPointer (4, GL_FLOAT, 0, colors_buff);
    // glTexCoordPointer (2, GL_FLOAT, 0, texture_buff);

    glDrawArrays (GL_TRIANGLES, 0, 2); // draw the 2 triangles

    glDisableClientState (GL_VERTEX_ARRAY);
    glDisableClientState (GL_NORMAL_ARRAY);
    glDisableClientState (GL_COLOR_ARRAY);*/
    // glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}

bool augmentation_widget::upload_to_gpu (const std::vector<float>& model_interleafed) {
    bool status = false;
    /*GLuint vertices_vbo = 0;
    GLuint colours_vbo  = 0;
    GLuint normals_vbo  = 0;

    glGenBuffers (1, &vertices_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vertices_vbo);
    glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), _faces.data (),
    GL_STATIC_DRAW);

    glGenBuffers (1, &colours_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
    glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), _faces_colors.data (),
    GL_STATIC_DRAW);

    glGenBuffers (1, &normals_vbo);
    glBindBuffer (GL_ARRAY_BUFFER, normals_vbo);
    glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), _faces_normals.data (),
    GL_STATIC_DRAW);


    _model_vao = 0;
    glGenVertexArrays (1, &_model_vao);
    glBindVertexArray (_model_vao);
    glBindBuffer (GL_ARRAY_BUFFER, vertices_vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer (GL_ARRAY_BUFFER, colours_vbo);
    glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer (GL_ARRAY_BUFFER, normals_vbo);
    glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray (0);
    glEnableVertexAttribArray (1);
    glEnableVertexAttribArray (2);
    */

    /* Data is stored like:
    glVertexPointer (3, GL_FLOAT, 0, _faces.data ());
    glNormalPointer (GL_FLOAT, 0, _faces_normals.data ());
    glColorPointer (4, GL_FLOAT, 0, _faces_colors.data ());
    glDrawArrays (GL_TRIANGLES, 0, face_count);*/
    return status;
}
