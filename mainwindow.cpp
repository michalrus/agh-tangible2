#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenu>
#include <QCloseEvent>

MainWindow::MainWindow(CvCapture *cam, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    camera(cam)
{
    // ui

    ui->setupUi(this);

    // initialize camImage

    QImage dummy(100, 100, QImage::Format_RGB32);
    camImage = dummy;
    ui->imageLabel->setPixmap(QPixmap::fromImage(camImage));

    // icons, tray

    this->setWindowIcon(QIcon(":/resource/icon.png"));
    this->setWindowTitle("Tangible2");
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    createActions();
    createTrayIcon();

    trayIcon->show();

    // timer

    startTimer(33); // 33 ms -> ~30 fps
}

MainWindow::~MainWindow()
{
    delete ui;
    delete trayIcon;
    delete trayMenu;
    delete open;
    delete quit;
}

void MainWindow::createActions() {
    open = new QAction(tr("&Open"), this);
    connect(open, SIGNAL(triggered()), this, SLOT(show()));

    quit = new QAction(tr("&Quit"), this);
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon() {
    trayMenu = new QMenu(this);

    trayMenu->addAction(open);
    trayMenu->addSeparator();
    trayMenu->addAction(quit);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayMenu);

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconClicked(QSystemTrayIcon::ActivationReason)));

    trayIcon->setIcon(QIcon(":/resource/icon.png"));
}

void MainWindow::trayIconClicked(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger)
        this->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (trayIcon->isVisible()) {
        trayIcon->showMessage(tr("Tangible2 still there!"),
                              tr("This application is still running. To quit, click this icon and select Quit."));
        hide();
        event->ignore();
    }
}

void MainWindow::timerEvent(QTimerEvent *) {
    IplImage *image = cvQueryFrame(camera);
    ui->imageLabel->setPixmap(toPixmap(image));
}

QPixmap MainWindow::toPixmap(IplImage *cvimage) {
    int cvIndex, cvLineStart;

    switch (cvimage->depth) {
        case IPL_DEPTH_8U:
            switch (cvimage->nChannels) {
                case 3:
                    if ( (cvimage->width != camImage.width()) || (cvimage->height != camImage.height()) ) {
                        QImage temp(cvimage->width, cvimage->height, QImage::Format_RGB32);
                        camImage = temp;
                    }
                    cvIndex = 0; cvLineStart = 0;
                    for (int y = 0; y < cvimage->height; y++) {
                        unsigned char red,green,blue;
                        cvIndex = cvLineStart;
                        for (int x = 0; x < cvimage->width; x++) {
                            red = cvimage->imageData[cvIndex+2];
                            green = cvimage->imageData[cvIndex+1];
                            blue = cvimage->imageData[cvIndex+0];

                            camImage.setPixel(x,y,qRgb(red, green, blue));
                            cvIndex += 3;
                        }
                        cvLineStart += cvimage->widthStep;
                    }
                    break;
                default:
                    qWarning("This number of channels is not supported\n");
                    break;
            }
            break;
        default:
            qWarning("This type of IplImage is not implemented\n");
            break;
    }

    return QPixmap::fromImage(camImage);

}
