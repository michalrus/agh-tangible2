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
    // 0. binaryzacja (potrzebna i do działania, i do kalibracji
    Mat binary = binarize(frame);

    // 1. znajdź markery (potrzebne i do działania, i do kalibracji
    findMarkers(binary);

    // 2. wyświetl wynik 0. i 1. w zakładce Debug
    // ważne: musimy skopiować dane do this->debug, ponieważ
    // po wyjściu z tej funkcji wszystkie jej lokalne macierze
    // zostaną (teoretycznie) zwolnione
    binary.copyTo(debug);

    if (calibrating) {
        // jeśli kalibrujemy w tej ramce:

        calibrate(binary); // to jest do zmiany -- kalibracja chce
                           // już dostać 4 rozpoznane prostokąty w
                           // rogach, a nie ramkę, ale to zaraz

        // następna ramka już nie będzie kalibracyjną
        calibrating = false;

        // jeśli kalibrujemy, to żadnego gestu nie ma
        return GNone;
    }
    else {
        // jeśli jednak nie kalibrujemy (czyli normalne wywołanie):

        // ... rozpoznaj gest z pozycji i klas wykrytych markerów

        // 3. zwróć wykryty gest albo GNone (jeśli żaden1)
        return GNone;
    }
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

        prevCols = frame.cols;
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

    // 1. znajdź kontury obiektów:

    using std::vector;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(binary, contours, hierarchy,
                 CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_KCOS);

    drawContours(binary, contours, -1, Scalar(255, 255, 255));

    if (!contours.size()) // avoid sigsegv ;)
        return;

    // 2. compare contours to our knowledge base (each-each)
}

const Mat& Processor::getDebug() const {
    // zwracamy obrazek-przetworzenie obrazka z kamery (podgląd do testowania jakichś przetworzeń etc.)
    return debug;
}

void Processor::doCalibrate() {
    calibrating = true;
}
