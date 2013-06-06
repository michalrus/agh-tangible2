#include "gesturehandler.h"

#include <QDebug>

GestureHandler::GestureHandler()
{
}

void GestureHandler::handle(Gesture gest) {
    switch (gest) {
    case GBack:     back();     break;
    case GForward:  forward();  break;
    default:                    break;
    }
}

void GestureHandler::back() {
    qDebug() << "Gesture: back()";

    // do sth
}

void GestureHandler::forward() {
    qDebug() << "Gesture: forward()";

    // do sth
}
