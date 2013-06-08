#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QSystemTrayIcon>

#include <opencv2/opencv.hpp>

#include "processor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void timerEvent(QTimerEvent*);
    
private:
    void createActions();
    void createTrayIcon();
    void setIcon();
    void closeEvent(QCloseEvent *);
    QImage mat2QImage(const cv::Mat& src);

    std::auto_ptr<Ui::MainWindow> ui;

    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *open;
    QAction *quit;

    int timerId;
    cv::VideoCapture capture;
    bool fromCamera;
    QString videoFile;
    Processor processor;

    void toggleCamera(bool);
    void toggleFromCamera(bool);

private slots:
    void on_trayIcon_clicked(QSystemTrayIcon::ActivationReason);
    void on_actionCalibrate_triggered();
    void on_actionCamera_toggled(bool);
    void on_actionDebug_toggled(bool);
    void on_actionFrom_file_toggled(bool);
    void on_actionFrom_cam0_toggled(bool);
    void on_actionReset_calibration_triggered();
};

#endif // MAINWINDOW_H
