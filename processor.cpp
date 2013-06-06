#include "processor.h"

using namespace cv;

Processor::Processor()
    : calibrating(false)
{
}

Gesture Processor::process(const Mat& frame) {
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

    // przykładowe przetworzenia
    Mat grayscale;
    cvtColor(frame, grayscale, CV_BGR2GRAY); // np. konwersja na odcienie szarości

    // reference opencv do C++ jest tutaj:
    // http://opencv.willowgarage.com/documentation/cpp/

    // jakieś przetworzenia do wyświetlenia w UI obok kamery
    // ważne: musimy skopiować dane do this->debug, ponieważ
    // po wyjściu z tej funkcji wszystkie jej lokalne macierze
    // zostaną zwolnione
    grayscale.copyTo(debug);

    return GNone; // albo jakiś inny gest na stoliku, jeśli wykryty
}

const Mat& Processor::getDebug() const {
    // zwracamy obrazek-przetworzenie obrazka z kamery (podgląd do testowania jakichś przetworzeń etc.)
    return debug;
}

void Processor::doCalibrate() {
    calibrating = true;
}
