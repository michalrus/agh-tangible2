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
    processor.cpp

HEADERS  += mainwindow.h \
    processor.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    resource.qrc

INCLUDEPATH += C:\\opencv\\build\\include

LIBS += -LC:\\opencv\\build\\x86\\vc11\\lib \
    -lopencv_core245 \
    -lopencv_highgui245 \
    -lopencv_imgproc245 \
    -lopencv_features2d245 \
    -lopencv_calib3d245
