#ifndef GESTUREHANDLER_H
#define GESTUREHANDLER_H

#include "processor.h"

class GestureHandler
{
public:
    GestureHandler();

    void handle(Gesture);

private:

    void back();
    void forward();
};

#endif // GESTUREHANDLER_H
