#include <QApplication>
#include <QFont>

#include "gui/mainwindow.h"

int main(int argc, char **argv)
{
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setApplicationName("Audio Slicer");
    app.setApplicationDisplayName("Audio Slicer");

#if defined(Q_OS_WIN)
    QFont font("Microsoft Yahei UI");
#else
    QFont font;
#endif
    font.setPixelSize(12);
    app.setFont(font);

    MainWindow window;
    window.show();

    return app.exec();
}