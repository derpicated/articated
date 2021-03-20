// main.cpp
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSurfaceFormat>

#include "frame_data_lister.hpp"
#include "shared/frame_data.hpp"
#include "vision/vision.hpp"

namespace {
void configureOpenGL (bool force_gles) {
    // Set GL version
    QSurfaceFormat glFormat;
    if (QOpenGLContext::openGLModuleType () == QOpenGLContext::LibGL && !force_gles) {
        // on desktop, require OpenGL 4.1
        glFormat.setVersion (4, 1);
    } else {
        // on mobile, require OpenGL-ES 3.0
        glFormat.setRenderableType (QSurfaceFormat::OpenGLES);
        glFormat.setVersion (3, 0);
    }
    glFormat.setProfile (QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat (glFormat);

    // Allow for easy resource sharing between QOpenGLContexts
    QCoreApplication::setAttribute (Qt::AA_ShareOpenGLContexts);
}

void configureQML () {
    // register Qt meta types
    qRegisterMetaType<FrameData> ();

    // register QML types
    qmlRegisterType<Vision> ("articated.vision", 1, 0, "Vision");
    qmlRegisterType<FrameDataLister> ("articated.tests.frameDataLister", 1, 0, "FrameDataLister");
}
} // namespace

int main (int argc, char* argv[]) {
    configureOpenGL (true);
    QApplication app (argc, argv);
    setlocale (LC_NUMERIC, "C");
    QCommandLineParser parser;

    // parse options
    parser.addHelpOption ();
    QCommandLineOption force_gles_option ("force-gles", "force usage of openGLES");
    parser.addOption (force_gles_option);
    parser.process (app);
//    bool force_gles = parser.isSet (force_gles_option);


    QQmlApplicationEngine engine;
    configureQML ();

    engine.load (QUrl (QStringLiteral ("qrc:/test/VisionTest.qml")));

    return app.exec ();
}
