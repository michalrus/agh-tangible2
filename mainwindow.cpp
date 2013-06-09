#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenu>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>

#if defined(Q_OS_WIN)
#   include <windows.h>
#else
#   error Unknown operating system, no specific SystemControl available.
#endif

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timerId(-1),
    fromCamera(false)
{
    // ui

    ui->setupUi(this);

    // icons, tray

    this->setWindowIcon(QIcon(":/resource/icon.png"));
    this->setWindowTitle("Tangible2");
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->move(50, 50);

    createActions();
    createTrayIcon();

    trayIcon->show();

    toggleFromCamera(true);

    // register systemControl system-wide hotkey: F9
#if defined(Q_OS_WIN)
    BOOL rv = RegisterHotKey(reinterpret_cast<HWND>(winId()), 0, 0x4000, VK_F9); // 0x4000 = MOD_NOREPEAT on Win7+
    if (!rv)
        qWarning() << "could not register F9 hotkey";
#endif
}

void MainWindow::createActions() {
    open = new QAction(tr("&Open"), this); // deleted automatically by Qt
    connect(open, SIGNAL(triggered()), this, SLOT(show()));

    quit = new QAction(tr("&Quit"), this); // deleted automatically by Qt
    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon() {
    trayMenu = new QMenu(this); // deleted automatically by Qt

    trayMenu->addAction(open);
    trayMenu->addSeparator();
    trayMenu->addAction(quit);

    trayIcon = new QSystemTrayIcon(this); // deleted automatically by Qt
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

    if (!capture.isOpened())
        return;

    if (!fromCamera) {
        if (capture.get(CV_CAP_PROP_POS_FRAMES) >= (capture.get(CV_CAP_PROP_FRAME_COUNT) - 1)) {
            // if a last frame of a video file
            capture.set(CV_CAP_PROP_POS_FRAMES, 0);
            return;
        }
    }

    try {
        capture >> frame;
    } catch (cv::Exception& e) {
        QMessageBox::warning(this, tr("Tangible2"), tr("Could not capture frame: ") + e.what(), QMessageBox::Ok);
        toggleFromCamera(true);
        return;
    }

    processor.process(frame, ui->actionControl_system->isChecked());

    const Mat &show = (ui->actionDebug->isChecked() ? processor.getDebug() : frame);

    ui->imageLabel->setPixmap(QPixmap::fromImage(mat2QImage(show)));
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result) {
    // process the native message
    // and if it's a system-wide f9 hotkey...
#if defined(Q_OS_WIN)
    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY && LOWORD(msg->lParam) == 0 && HIWORD(msg->lParam) == VK_F9) {
        ui->actionControl_system->toggle();
        return true; // do not pass it to Qt handler
    }
#endif
    return false;
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
    toggleCamera(set);
}

void MainWindow::on_actionDebug_toggled(bool set)
{
    toggleCamera(!set);
}

void MainWindow::toggleCamera(bool set) {
    ui->actionDebug->setChecked(!set);
    ui->actionCamera->setChecked(set);
}

void MainWindow::on_actionFrom_file_toggled(bool set)
{
    toggleFromCamera(!set);
}

void MainWindow::on_actionFrom_cam0_toggled(bool set)
{
    toggleFromCamera(set);
}

void MainWindow::toggleFromCamera(bool set) {
    if (!set) { // if from file
        if (fromCamera) {
            videoFile = QFileDialog::getOpenFileName(this, tr("Select video file"), "", tr("Video files (*.avi)"));
            if (!videoFile.isEmpty()) {
                killTimer(timerId);

                capture.release();
                capture.open(videoFile.toStdString());
                if (!capture.isOpened()) {
                    QMessageBox::warning(this, tr("Tangible2"), tr("Could not open ") + videoFile, QMessageBox::Ok);
                }
                else {
                    fromCamera = false;

                    double fps = capture.get(CV_CAP_PROP_FPS);
                    timerId = startTimer(1000.0 / fps);
                }
            }
        }
    }
    else { // if from cam0
        if (!fromCamera) {
            capture.release();
            capture.open(0); // default camera
            if (!capture.isOpened())
                QMessageBox::warning(this, tr("Tangible2"), tr("Could not open a default camera."), QMessageBox::Ok);
            else {
                killTimer(timerId);
                timerId = startTimer(33); // 33 ms -> ~30 fps
            }
        }
        fromCamera = true;
    }

    ui->actionFrom_file->setChecked(/*capture.isOpened() &&*/ !fromCamera);
    ui->actionFrom_cam0->setChecked(capture.isOpened() && fromCamera);
}

void MainWindow::on_actionReset_calibration_triggered()
{
    processor.doReset();
}

void MainWindow::on_actionControl_system_toggled(bool set)
{
    if (set) {
        trayIcon->showMessage(tr("Tangible2 takes over!"),
                              tr("You can now control your system (mouse, keyboard) with the Tangible Table.\n\nHit F9 to stop."));
    }
    else {
        trayIcon->showMessage(tr("Tangible2 gives up control."),
                              tr("You no longer can control your system with the Tangible Table.\n\nHit F9 to start over."));
    }
}
