#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSystemTrayIcon>

#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(CvCapture *camera, QWidget *parent = 0);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent*);
    
private:
    void createActions();
    void createTrayIcon();
    void setIcon();
    void closeEvent(QCloseEvent *);

    Ui::MainWindow *ui;

    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *open;
    QAction *quit;

    CvCapture *camera;
    QImage camImage;

    QPixmap toPixmap(IplImage *cvimage);

private slots:
    void trayIconClicked(QSystemTrayIcon::ActivationReason);
};

#endif // MAINWINDOW_H
