#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSystemTrayIcon>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "processor.h"
#include "gesturehandler.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent*);
    
private:
    void createActions();
    void createTrayIcon();
    void setIcon();
    void closeEvent(QCloseEvent *);
    QPixmap toPixmap(IplImage *cvimage);
    QImage mat2QImage(const cv::Mat& src);

    Ui::MainWindow *ui;

    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *open;
    QAction *quit;

    cv::VideoCapture capture;
    Processor processor;
    GestureHandler gestureHandler;

private slots:
    void on_trayIcon_clicked(QSystemTrayIcon::ActivationReason);
    void on_actionCalibrate_triggered();
    void on_actionCamera_toggled(bool arg1);
    void on_actionDebug_toggled(bool arg1);
};

#endif // MAINWINDOW_H
