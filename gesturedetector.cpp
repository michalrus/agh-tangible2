#include "gesturedetector.h"

#include <QDebug>

GestureDetector::GestureDetector()
    : lastClickFrame(0)
{
}

void GestureDetector::handle(bool control, const std::vector<Marker>& frame) {
    // tymczasowo
    markers.clear();
    markers = frame;
    return;

    if (!control) // jeśli kontrola jest wyłączona w GUI...
        return;

    QString window = systemControl.getCurrentWindowTitle();

/*    if (type == "e.triangle")
        return systemControl.sendMouseXY(x, y);

    if (type == "triangle" && frameNumber - lastClickFrame > 20) {
        // jeśli osobnik "zatkał" tamten trójkąt
        // i jeśli ostatnio klikaliśmy przynajmniej 20 sekund temu: kliknij lewym
        lastClickFrame = frameNumber;
        systemControl.sendMouseXY(x, y);
        return systemControl.sendMouseClick();
    }

    if (window.contains("chrome", Qt::CaseInsensitive) && type == "circle") {
        // jeśli to Google Chrome, to możemy użyć pozycji kółka w pionie do scrollowania
        double vel = -0.3 * (y * 2.0 - 1.0); // przeskaluj [0;1] na 0.3*[1;-1]
        return systemControl.sendMouseScroll(vel);
    }*/
}

const std::vector<Marker>& GestureDetector::getMarkers() const {
    return markers;
}
