#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel window("Simulador A*");
    window.setAlignment(Qt::AlignCenter);
    window.setWindowTitle("Simulador A*");
    window.resize(900, 600);
    window.show();

    return app.exec();
}
