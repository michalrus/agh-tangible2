#include "gesturedetector.h"

#include <QDebug>

GestureDetector::GestureDetector()
    : lastClickFrame(0),
      frameNumber(0)
{
}

void GestureDetector::handle(bool control, const std::vector<Marker>& frame) {
    // tymczasowo -- tu ma być zaimplementowane branie pod uwagę historii
    markers.clear();
    markers = frame;

    if (control)
        detectGestures();
}

const std::vector<Marker>& GestureDetector::getMarkers() const {
    return markers;
}

void GestureDetector::detectGestures() {
    QString window = systemControl.getCurrentWindowTitle();

    frameNumber++;

    for (size_t idx = 0; idx < markers.size(); idx++) {
        Marker& m = markers[idx];

        if (m.name == "e.triangle")
            return systemControl.sendMouseXY(m.x, m.y);

        if (m.name == "triangle" && frameNumber - lastClickFrame > 20) {
            // jeśli osobnik "zatkał" tamten trójkąt
            // i jeśli ostatnio klikaliśmy przynajmniej 20 klatek temu: kliknij lewym
            lastClickFrame = frameNumber;
            systemControl.sendMouseXY(m.x, m.y);
            return systemControl.sendMouseClick();
        }

        if (window.contains("chrome", Qt::CaseInsensitive) && m.name == "circle") {
            // jeśli to Google Chrome, to możemy użyć pozycji kółka w pionie do scrollowania
            double vel = -0.3 * (m.y * 2.0 - 1.0); // przeskaluj [0;1] na 0.3*[1;-1]
            return systemControl.sendMouseScroll(vel);
        }
    }
}
