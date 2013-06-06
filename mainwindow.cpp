#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenu>
#include <QCloseEvent>
#include <QMessageBox>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    capture(0) // default camera
{
    if (!capture.isOpened()) {
        QMessageBox::critical(this, tr("Tangible2"), tr("Could not open a camera."), QMessageBox::Ok);
        qApp->quit();
    }

    // ui

    ui->setupUi(this);

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

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_trayIcon_clicked(QSystemTrayIcon::ActivationReason)));

    trayIcon->setIcon(QIcon(":/resource/icon.png"));
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
    Mat frame;
    capture >> frame;

    Gesture gest = processor.process(frame);

    const Mat &show = (ui->actionDebug->isChecked() ? processor.getDebug() : frame);

    ui->imageLabel->setPixmap(QPixmap::fromImage(mat2QImage(show)));
}

QImage MainWindow::mat2QImage(const cv::Mat &src) {
    QImage dest(src.cols, src.rows, QImage::Format_ARGB32);

    for (int y = 0; y < src.rows; ++y) {
        QRgb *destrow = (QRgb*)dest.scanLine(y);

        for (int x = 0; x < src.cols; ++x) {
            uchar r, g, b;

            if (src.type() == CV_8UC3) { // 8-bit unsigned int * 3 channels
                const Vec3b& vec = src.at<Vec3b>(y,x);
                r = vec(2);
                g = vec(1);
                b = vec(0);
            }
            else if (src.type() == CV_8UC1) { // 8-bit unsigned int * 1 channel
                r = g = b = src.at<uchar>(y,x);
            }
            else { // unsupported type, display green
                r = 0; g = 255; b = 0;
            }

            destrow[x] = qRgba(r, g, b, 255);
        }
    }

    return dest;
}

void MainWindow::on_trayIcon_clicked(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger)
        this->show();
}

void MainWindow::on_actionCalibrate_triggered()
{
    processor.doCalibrate();
}

void MainWindow::on_actionCamera_toggled(bool set)
{
    ui->actionDebug->setChecked(!set);
}

void MainWindow::on_actionDebug_toggled(bool set)
{
    ui->actionCamera->setChecked(!set);
}
