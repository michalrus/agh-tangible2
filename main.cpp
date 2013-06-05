#include "mainwindow.h"
#include <QApplication>
#include <opencv/cv.h>
#include <opencv/highgui.h>

int main(int argc, char *argv[])
{
    // połączenie z domyślną kamerą
    CvCapture *camera = cvCreateCameraCapture(0);
    assert(camera);
    // sprawdzenie czy działa
    IplImage *image = cvQueryFrame(camera);
    assert(image);

    // inicjalizacja aplikacji
    QApplication a(argc, argv);
    // inicjalizacja okna, przekazanie połączenia z kamerą
    MainWindow w(camera);
    w.show();
    
    int retval = a.exec();

    // close camera connection
    cvReleaseCapture(&camera);

    return retval;
}
