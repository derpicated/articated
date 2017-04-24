#include <QApplication>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>

#include "window.hpp"

int main (int argc, char* argv[]) {
    QSurfaceFormat glFormat;
    if (QOpenGLContext::openGLModuleType () == QOpenGLContext::LibGL) {
        // on desktop, require opengl 4.3
        glFormat.setVersion (4, 3);
    } else {
        // on mobile, require opengles 3.0
        glFormat.setVersion (3, 0);
    }

    glFormat.setProfile (QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat (glFormat);

    // create the main app window
    QApplication app (argc, argv);
    Window window;

    window.setWindowTitle ("ARticated");

    window.show ();
    // QQmlApplicationEngine engine;
    // engine.load (QUrl (QStringLiteral ("qrc:/main.qml")));

    return app.exec ();
}
