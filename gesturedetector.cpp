#include "gesturedetector.h"

#include <QDebug>
#include <iostream>

GestureDetector::GestureDetector()
{
}

void GestureDetector::handle(size_t frameNumber, std::string type, double x, double y) {
    QString window = systemControl.getCurrentWindowTitle();

    if (0 && type == "e.triangle")
        return systemControl.sendMouseXY(x, y);
}
