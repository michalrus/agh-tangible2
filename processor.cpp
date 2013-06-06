#include "processor.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

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

    // macierze można też podawać jednocześnie jako źródło i docelową...
    // ale nie wiem na ile to jest bezpieczne (chyba nadpisuje macierz
    // źródłową w trakcie działania, więc przy binaryzacji spoko, ale jak
    // w innym algorytmie będzie potrzeba się odwoływać do już zmienionych
    // pikseli w źrodle? nie wiem.
    threshold(grayscale, grayscale, 85, 255, 0);

    // bezpieczniejsza wersja (?)
    /*
    Mat binary;
    threshold(grayscale, binary, 85, 255, 0);
    binary.copyTo(debug); // wtedy już bez tego kopiowania niżej, bo bez sensu ;)
    */

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
