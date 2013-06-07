#include "gesturedetector.h"

#include <QDebug>
#include <iostream>

GestureDetector::GestureDetector()
{
}

void GestureDetector::handle(size_t frameNumber, std::string type, double x, double y) {
    if (type == "e.triangle") {
        systemControl.setMouseXY(x, y);
    }

    //std::cout << frameNumber << ": " << type << " @ (" << x << "," << y << ")" << std::endl;
}
