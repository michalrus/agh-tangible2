#include "processor.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace cv;

Processor::Processor()
    : calibrating(false),
      prevCols(0)
{
}

Gesture Processor::process(const Mat& frame) {
    // reference opencv do C++ jest tutaj:
    // http://opencv.willowgarage.com/documentation/cpp/

    // binaryzacja (potrzebna i do kształtów, i do kalibracji
    Mat binary = binarize(frame);

    // jeśli kalibrujemy w tej ramce:
    if (calibrating) {
        calibrate(binary);
        calibrating = false;
        return GNone;
    }

    // jeśli jednak nie kalibrujemy (czyli normalne wywołanie):

    // 1. znajdź markery
    findMarkers(binary);

    // 2. wyświetl jakiś obraz w zakładce Debug
    // ważne: musimy skopiować dane do this->debug, ponieważ
    // po wyjściu z tej funkcji wszystkie jej lokalne macierze
    // zostaną (teoretycznie) zwolnione
    binary.copyTo(debug);

    // 3. zwróć wykryty gest albo GNone (na obecnym etapie: tylko
    // GNone...)
    return GNone;
}

Mat Processor::binarize(const Mat& frame) {
    // ustaw odpowiednio strel, jeśli zmienił się rozmiar ramki od ostatniej
    if (frame.cols != prevCols) {
        int strelOpenSize = frame.cols * 0.01;
        if (strelOpenSize % 2 == 0)
            strelOpenSize++; // musi być nieparzysty
        if (strelOpenSize < 3)
            strelOpenSize = 3; // najmniejszy to 3
        strelOpen = getStructuringElement(CV_SHAPE_CROSS, Size(strelOpenSize, strelOpenSize));

        int strelCloseSize = strelOpenSize * 1.66;
        if (strelCloseSize % 2 == 0)
            strelCloseSize++; // musi być nieparzysty
        if (strelCloseSize < 3)
            strelCloseSize = 3; // najmniejszy to 3
        strelClose = getStructuringElement(CV_SHAPE_ELLIPSE, Size(strelCloseSize, strelCloseSize));
    }

    Mat binary;

    // konwersja na odcienie szarości (typ unsigned 8-bit int (uchar))
    cvtColor(frame, binary, CV_BGR2GRAY);

    // rozmiar bloku do adaptywnej binaryzacji jako 10% szerokości ramki
    int blockSize = 0.10 * frame.cols;
    if (blockSize % 2 == 0)
        blockSize++; // rozmiar bloku musi być nieparzysty

    // adaptacyjna binaryzacja
    adaptiveThreshold(binary, binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, blockSize, 0);

    // otwórz śmieci
    morphologyEx(binary, binary, CV_MOP_OPEN, strelOpen);

    // zamknij figury
    morphologyEx(binary, binary, CV_MOP_CLOSE, strelClose);

    return binary;
}

void Processor::calibrate(const Mat& img) {
    // przeprowadź kalibrację na ramce frame

    // oczywiście tutaj też możemy ustawiać sobie debug
    // (ale pewnie nie będzie go widać przez tę jedną ramkę)

    // a z drugiej strony nikt nie każe w process()
    // wyłączać kalibracji po jednej ramce (linijka "calibrating = false;")
    // można wyłączyć np. dopiero po 50 (czyli jakieś 1.5 sekundy)
}

void Processor::findMarkers(const Mat& binary) {
    // staraj się wykryć jakieś gesty/położenia markerów
    // zarówno z tej ramki jak i ew. z danych z poprzednich
    // obiekt Processor może mieć przecież jakąś pamięć
    // ale to już jak chcecie
}

const Mat& Processor::getDebug() const {
    // zwracamy obrazek-przetworzenie obrazka z kamery (podgląd do testowania jakichś przetworzeń etc.)
    return debug;
}

void Processor::doCalibrate() {
    calibrating = true;
}
