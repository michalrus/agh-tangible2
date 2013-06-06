#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // inicjalizacja aplikacji
    QApplication a(argc, argv);
    // inicjalizacja okna, przekazanie połączenia z kamerą
    MainWindow w;
    w.show();

    return a.exec();
}
