#include "gesturedetector.h"

#include <QDebug>
#include <iostream>

GestureDetector::GestureDetector()
{
}

void GestureDetector::handle(std::string type, double x, double y) {
    // std::cout << type << " @ (" << x << "," << y << ")" << std::endl;
}

void GestureDetector::back() {
    qDebug() << "Gesture: back()";

    // do sth
}

void GestureDetector::forward() {
    qDebug() << "Gesture: forward()";

    // do sth
}
