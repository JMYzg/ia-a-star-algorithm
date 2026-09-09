#include <QApplication>
#include <QFile>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile styleFile(":/styles/app.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));

    MainWindow window;
    window.show();

    return app.exec();
}
