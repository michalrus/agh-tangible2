#include "processor.h"

#include <QImage>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>

//#include <opencv2/imgproc.hpp>
//#include <opencv2/imgproc/types_c.h>

using namespace cv;

Processor::Processor()
    : calibrating(false),
      resetting(false),
      prevCols(0)
{
    buildKBContours();
}

Gesture Processor::process(const Mat& frame) {
    if (resetting) {
        // jeśli user zażyczył sobie resetu macierzy kalibracji

        //...

        resetting = false;
    }

    markers.clear();
    frameArea = frame.cols * frame.rows;

    // 0. binaryzacja (potrzebna i do działania, i do kalibracji
    Mat binary = binarize(frame);

    // 1. znajdź markery (potrzebne i do działania, i do kalibracji
    findMarkers(binary);

    // 2. wyświetl kopię ramki w zakładce Debug
    // ważne: musimy skopiować dane do this->debug, ponieważ
    // po wyjściu z tej funkcji wszystkie jej lokalne macierze
    // zostaną (teoretycznie) zwolnione
    // (a do frame nie możemy pisać)
    frame.copyTo(debug);

    // 2.a. narysuj markery na debug
    for (size_t i = 0; i < markers.size(); i++)
        markers[i].drawOn(debug);

    if (calibrating) {
        // jeśli kalibrujemy w tej ramce:
        calibrate();

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

void Processor::calibrate() {
    // 1. sprawdźmy, czy mamy tylko i wyłącznie 4 markery

    if (markers.size() != 4) {
        QMessageBox::warning(qApp->desktop()->screen(), QObject::tr("Tangible2"), QObject::tr("Calibration requires exactly 4 markers in the corners of the table."), QMessageBox::Ok);
        return;
    }

    // 2. zapiszmy ich pozycje:

    Point a = markers[0].getCenter();
    Point b = markers[1].getCenter();
    Point c = markers[2].getCenter();
    Point d = markers[3].getCenter();
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

    // jeszcze:
    // użyć approxPolyDP? żeby "wygładzić" kształty -- jeśli miałby
    // być jakiś problem z rozpoznawaniem po dodaniu kilku nowych

    vector<std::string> recognizedShapes;

    for (size_t idx = 0; idx < contours.size(); idx++) {
        double bestMatch = INFINITY;
        int bestI = -1;
        for (size_t i = 0; i < knownContours.size(); i++) {
            double match = matchShapes(contours[idx], knownContours[i].contour, CV_CONTOURS_MATCH_I1, 0);
            if (bestI == -1 || match < bestMatch) {
                bestI = i;
                bestMatch = match;
            }
        }

        recognizedShapes.push_back(knownContours[bestI].name);
    }

    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
        // jeśli pole konturu < 5%^2 pola ramki, nie uwzględniaj go jako markera
        // ... czyli ostateczna rozgrywka ze "śmieciami"
        // to samo gdy > 30%^2
        double area = contourArea(contours[idx]);
        if (area < 0.05 * 0.05 * frameArea || area > 0.30 * 0.30 * frameArea)
            continue;

        std::string name = recognizedShapes[idx];
        if (hierarchy[idx][2] >= 0) { // if this contour has a hole (child) in it
            // sprawdź czy pole dziecka nie jest za małe w stosunku
            // do rodzica (czy to nie śmieć)
            double holeArea = contourArea(contours[hierarchy[idx][2]]);
            if (holeArea > 0.25 * area)
                name = "e." + name;
        }
        markers.push_back(Marker(name, contours[idx]));
    }

//    std::cout << std::endl << "-- new frame" << std::endl;
//    for (int i = 0; i < recognizedShapes.size(); i++)
//        std::cout << i << " is a " << recognizedShapes[i] << std::endl;
}

void Processor::buildKBContours() {
    knownContours.push_back(Contour("circle",    ":/resource/circle.png"));
    knownContours.push_back(Contour("cross",     ":/resource/cross.png"));
    knownContours.push_back(Contour("triangle",  ":/resource/triangle.png"));
    knownContours.push_back(Contour("triangle",  ":/resource/triangle_rounded.png"));

    // niektóre trójkąty są zaokrąglone, a niektóre ostre
    // dlatego potrzeba dwa wzorce

    // niestety jak dodamy prostokąt, to przy niedoświetleniu
    // czasem myli koło z prostokątem... prostokąty były używane
    // tylko do kalibracji, a do niej przecież wystarczą dowolne
    // 4 markery, więc to poniżej jest wyłączone

    //knownContours.push_back(Contour("rectangle", ":/resource/rectangle.png"));
}

const Mat& Processor::getDebug() const {
    // zwracamy obrazek-przetworzenie obrazka z kamery (podgląd do testowania jakichś przetworzeń etc.)
    return debug;
}

void Processor::doCalibrate() {
    calibrating = true;
}

void Processor::doReset() {
    resetting = true;
}
