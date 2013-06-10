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
      warping(false),
      prevCols(0),
      framesPassed(0)
{
    buildKBContours();
}

void Processor::process(const Mat& frame, bool control) {
    // 1. inicjalizacja
    markers.clear();
    frameCenter = Point(0.5 * frame.cols, 0.5 * frame.rows);
    frameArea = frame.cols * frame.rows;

    // 2. wyświetl kopię ramki w zakładce Debug
    // ważne: musimy skopiować dane do this->debug, ponieważ
    // po wyjściu z tej funkcji wszystkie jej lokalne macierze
    // zostaną (teoretycznie) zwolnione
    // (a do frame nie możemy pisać)
    // od tej pory działamy tylko na debug
    //frame.copyTo(debug);
    cv::flip(frame, debug, 1);

    // 3. jeśli user dokonał już kalibracji, transformuj każdą klatkę
    if (warping) {
        // zmień perspektywę
        warpPerspective(debug, debug, warpPerspectiveTransformMatrix, debug.size(), INTER_LINEAR, BORDER_REPLICATE);
    }

    // 4. binaryzacja (potrzebna i do działania, i do kalibracji
    Mat binary = binarize(debug);

    // 5. znajdź markery (potrzebne i do działania, i do kalibracji
    findMarkers(binary);

    // 6. narysuj markery na debug
    for (size_t i = 0; i < markers.size(); i++)
        markers[i].drawOn(debug);

    // 7. sprawdź co dalej: kalibracja/gesty
    if (calibrating) {
        // jeśli kalibrujemy w tej ramce:
        calibrate();

        // następna ramka już nie będzie kalibracyjną
        calibrating = false;
    }
    else {
        // jeśli jednak nie kalibrujemy (czyli normalne wywołanie):

        // rozpoznaj gest z pozycji i klas wykrytych markerów
        for (size_t i = 0; i < markers.size(); i++) {
            Point pos = markers[i].getCenterOnFrame();
            gestureDetector.handle(control, framesPassed, markers[i].getName(), (double)pos.x / frame.cols, (double)pos.y / frame.rows);
        }

        framesPassed++;
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
    int blockSize = 0.15 * frame.cols;
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

/**
 * @brief comparePointsClockWise
 * @param a
 * @param b
 * @return true gdy a < b
 */

/**
 * @brief ComparePointsClockWise porównuje dwa punkty (który zatacza większy kąt względem środka ramki)
 */
struct ComparePointsClockWise : public std::binary_function<Point,Point,bool>
{
    Point center;
    ComparePointsClockWise(const Point& center_) : center(center_) {}
    inline bool operator()(const Point& a, const Point& b) {
        double atanA = atan2(a.y - center.y, a.x - center.x);
        double atanB = atan2(b.y - center.y, b.x - center.x);
        return (atanA > atanB);
    }
};

void Processor::calibrate() {
    using namespace std;

    // 0. sprawdźmy, czy mamy tylko i wyłącznie 4 markery

    if (markers.size() != 4) {
        QMessageBox::warning(qApp->desktop()->screen(), QObject::tr("Tangible2"), QObject::tr("Calibration requires exactly 4 markers in the corners of the table."), QMessageBox::Ok);
        return;
    }

    // 1. sprawdźmy czy kalibracja nie została już wykonana

    if (warping) {
        QMessageBox::warning(qApp->desktop()->screen(), QObject::tr("Tangible2"), QObject::tr("Calibration already done. Use \"Reset calibration\" first."), QMessageBox::Ok);
        return;
    }

    // 2. zapiszmy ich pozycje w wektorze:

    vector<Point> p;
    for (size_t i = 0; i < markers.size(); i++)
        p.push_back(markers[i].getCenterOnFrame());

    // 3. do transformacji potrzebujemy mieć je w porządku zegarowym zaczynając of left-upper

    sort(p.begin(), p.end(), ComparePointsClockWise(frameCenter));

    // 4. w których punktach chcemy mieć obecne rogi zaznaczone markerami

    // margines dookoła ramki: 5% jej szerokości
    int w = frameCenter.x * 2;
    int h = frameCenter.y * 2;
    int margin = 0.05 * w;

    cv::Point2f dest_points[4];
    dest_points[0] = Point(margin, h - margin);
    dest_points[1] = Point(w - margin, h - margin);
    dest_points[2] = Point(w - margin, margin);
    dest_points[3] = Point(margin, margin);

    cv::Point2f src_points[4];
    src_points[0] = p[0];
    src_points[1] = p[1];
    src_points[2] = p[2];
    src_points[3] = p[3];

    warpPerspectiveTransformMatrix = getPerspectiveTransform(src_points, dest_points);

    warping = true;
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
    warping = false;
}
