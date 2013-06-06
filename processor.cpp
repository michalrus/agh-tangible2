#include "processor.h"

using namespace cv;

Processor::Processor()
    : calibrating(false)
{
}

Gesture Processor::process(const Mat& frame) {
    if (calibrating) {
        // jeśli kalibrujemy w tej ramce:
        calibrate(frame);
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

void Processor::calibrate(const Mat& frame) {
    // przeprowadź kalibrację na ramce frame

    // oczywiście tutaj też możemy ustawiać sobie debug
    // (ale pewnie nie będzie go widać przez tę jedną ramkę)

    // a z drugiej strony nikt nie każe w process()
    // wyłączać kalibracji po jednej ramce (linijka "calibrating = false;")
    // można wyłączyć np. dopiero po 50 (czyli jakieś 1.5 sekundy)
}

const Mat& Processor::getDebug() const {
    // zwracamy obrazek-przetworzenie obrazka z kamery (podgląd do testowania jakichś przetworzeń etc.)
    return debug;
}

void Processor::doCalibrate() {
    calibrating = true;
}
