#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSurfaceFormat>

#include "augmentation_widget/augmentation_view.hpp"
#include "vision/vision.hpp"

namespace {
void configureOpengl (bool force_gles) {
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

    QCoreApplication::setAttribute (Qt::AA_ShareOpenGLContexts);
}
} // namespace

int main (int argc, char* argv[]) {
    QCommandLineParser parser;
    parser.addHelpOption ();
    parser.setApplicationDescription (
    "ARticated: an augmented reality application");
    QCommandLineOption force_gles_option ("force-gles", "force usage of openGLES");
    parser.addOption (force_gles_option);

    // parse arguments
    QStringList arguments;
    for (int i = 0; i < argc; ++i) {
        arguments.append (argv[i]);
    }
    parser.process (arguments);

    // configure opengl
    bool force_gles = parser.isSet (force_gles_option);
    configureOpengl (force_gles);

    // create the main app & window
    QGuiApplication app (argc, argv);
    setlocale (LC_NUMERIC, "C");
    QQmlApplicationEngine engine;

    // register custom qml components
    qmlRegisterType<Vision> ("articated.vision", 1, 0, "Vision");
    qmlRegisterType<AugmentationView> (
    "articated.augmentation.augmentation_view", 1, 0, "AugmentationView");
    engine.load (QUrl (QStringLiteral ("qrc:/qml/Main.qml")));

    return app.exec ();
}
