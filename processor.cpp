#include "processor.h"

Processor::Processor()
    : calibrating(false),
      debug(NULL)
{
}

Gesture Processor::process(IplImage *frame) {
    if (calibrating) {
        // przeprowadź kalibrację na aktualnej ramce
        calibrating = false;
        return GNone;
    }

    // jeśli jednak nie kalibrujemy (czyli normalne wywołanie):

    // staraj się wykryć jakieś gesty/położenia markerów
    // zarówno z tej ramki jak i ew. z danych z poprzednich
    // obiekt Processor może mieć przecież pamięć
    // ale to już jak chcecie

    //debug = ... jakieś przetworzenia do wyświetlenia obok kamery

    return GNone; // albo jakiś inny gest, jeśli wykryty
}

IplImage* Processor::getDebug() {
    // zwracamy obrazek-przetworzenie obrazka z kamery (podgląd do testowania jakichś przetworzeń etc.)
    // ew. NULL, jeśli nie ma już potrzeby niczego podglądać
    return debug;
}

void Processor::doCalibrate() {
    calibrating = true;
}
