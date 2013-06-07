#-------------------------------------------------
#
# Project created by QtCreator 2013-06-05T13:02:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tangible2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    processor.cpp \
    gesturehandler.cpp \
    contour.cpp \
    marker.cpp

HEADERS  += mainwindow.h \
    processor.h \
    gesturehandler.h \
    contour.h \
    marker.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    resource.qrc

RC_FILE = tangible2.rc

INCLUDEPATH += C:/opencv/include

LIBS += -LC:/opencv/lib \
    -lopencv_calib3d245.dll \
    -lopencv_contrib245.dll \
    -lopencv_core245.dll \
    -lopencv_features2d245.dll \
    \
    -lopencv_highgui245.dll \
    -lopencv_imgproc245.dll \
    -lopencv_legacy245.dll \
    -lopencv_ml245.dll \
    \
    -lopencv_objdetect245.dll \
    \
    -lopencv_video245.dll
