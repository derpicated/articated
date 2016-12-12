#include <QApplication>
#include <QQmlApplicationEngine>

#include "window.h"

int main (int argc, char* argv[]) {
    QApplication app (argc, argv);
    Window window;

    window.setWindowTitle ("ARticated");

    window.show ();
    // QQmlApplicationEngine engine;
    // engine.load (QUrl (QStringLiteral ("qrc:/main.qml")));

    return app.exec ();
}
