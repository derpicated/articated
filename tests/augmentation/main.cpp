// main.cpp
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSurfaceFormat>

#include "augmentation_widget/augmentation_view.hpp"

int main (int argc, char* argv[]) {
    QApplication app (argc, argv);
    setlocale (LC_NUMERIC, "C");
    QCommandLineParser parser;

    // parse options
    parser.addHelpOption ();
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

    QQmlApplicationEngine engine;

    QDir path (":/3D_models/");
    QStringList model_files = path.entryList (QDir::Files);
    engine.rootContext ()->setContextProperty (
    "modelFilesList", QVariant::fromValue (model_files));

    qmlRegisterType<AugmentationView> (
    "articated.augmentation.augmentation_view", 1, 0, "AugmentationView");
    engine.load (QUrl (QStringLiteral ("qrc:/test/AugmentationTest.qml")));

    return app.exec ();
}
