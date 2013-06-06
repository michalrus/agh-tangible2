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
    gesturehandler.cpp

HEADERS  += mainwindow.h \
    processor.h \
    gesturehandler.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    resource.qrc

INCLUDEPATH += C:\\opencv\\build\\include

LIBS += -LD:\\opencv\\builds\\lib \
    -lopencv_core249.dll \
    -lopencv_video249.dll \
    -lopencv_ml249.dll \
    -lopencv_objdetect249.dll \
    -lopencv_contrib249.dll \
    -lopencv_legacy249.dll \
    -lopencv_highgui249.dll \
    -lopencv_imgproc249.dll \
    -lopencv_features2d249.dll \
    -lopencv_calib3d249.dll
