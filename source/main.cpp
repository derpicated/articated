#include <QApplication>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSurfaceFormat>

#include "window.hpp"

int main (int argc, char* argv[]) {
    QApplication app (argc, argv);
    setlocale (LC_NUMERIC, "C");
    QCommandLineParser parser;

    // parse options
    parser.addHelpOption ();
    parser.setApplicationDescription (
    "ARticated: an augmented reality application");
    QCommandLineOption force_gles_option ("force-gles", "force usage of openGLES");
    parser.addOption (force_gles_option);
    parser.process (app);
    bool force_gles = parser.isSet (force_gles_option);

    // set GL version
    QSurfaceFormat glFormat;
    if (QOpenGLContext::openGLModuleType () == QOpenGLContext::LibGL && !force_gles) {
        // on desktop, require opengl 4.1
        glFormat.setVersion (4, 1);
    } else {
        // on mobile, require opengles 3.0
        glFormat.setRenderableType (QSurfaceFormat::OpenGLES);
        glFormat.setVersion (3, 0);
    }

    glFormat.setProfile (QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat (glFormat);

    // create the main app window
    QQmlApplicationEngine engine;
    engine.load (QUrl (QStringLiteral ("qrc:/qml/main.qml")));

    return app.exec ();
}
