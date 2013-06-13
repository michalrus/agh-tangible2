#include "processor.h"

#include <QImage>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>

using namespace cv;

Processor::Processor()
    : calibrating(false),
      warping(false),
      prevCols(0)
{
    buildKBContours();
}

void Processor::process(const Mat& frame, bool control) {
    using std::vector;

    // 1. inicjalizacja
    frameCenter = Point(0.5 * frame.cols, 0.5 * frame.rows);
    frameArea = frame.cols * frame.rows;

    // 2. wyświetl kopię ramki w zakładce Debug
    // ważne: musimy skopiować dane do this->debug, ponieważ
    // po wyjściu z tej funkcji wszystkie jej lokalne macierze
    // zostaną (teoretycznie) zwolnione
    // (a do frame nie możemy pisać)
    // od tej pory działamy tylko na debug
    frame.copyTo(debug);
    //cv::flip(debug, debug, 1);

    // 3. jeśli user dokonał już kalibracji, transformuj każdą klatkę
    if (warping) {
        // zmień perspektywę
        warpPerspective(debug, debug, warpPerspectiveTransformMatrix, debug.size(), INTER_LINEAR, BORDER_REPLICATE);
    }

    // konwersja na odcienie szarości (typ unsigned 8-bit int (uchar))
    Mat grayscale;
    cvtColor(frame, grayscale, CV_BGR2GRAY);

    // 4. binaryzacja (potrzebna i do działania, i do kalibracji
    Mat binary = binarize(grayscale);

    // 5. znajdź markery tej konkretnej ramki
    vector<Marker> markers = findMarkers(binary, grayscale);

    // 6. każ mózgowi operacji porównać je z poprzednimi stanami
    //    -- lepsze śledzenie -- a oprócz tego ew zareagować na
    //    rozpoznane gesty
    gestureDetector.handle(control, markers);

    // 6. narysuj te lepsze markery na debug
    const vector<Marker>& betterMarkers = gestureDetector.getMarkers();
    for (size_t i = 0; i < betterMarkers.size(); i++)
        drawMarker(debug, betterMarkers[i]);

    // 7. sprawdź czy aby nie trzeba policzyć macierzy kalibracji
    if (calibrating) {
        // jeśli kalibrujemy w tej ramce:
        calibrate(betterMarkers);

        // następna ramka już nie będzie kalibracyjną
        calibrating = false;
    }
}

Mat Processor::binarize(const Mat& grayscale) {
    // ustaw odpowiednio strel, jeśli zmienił się rozmiar ramki od ostatniej
    if (grayscale.cols != prevCols) {
        int strelOpenSize = grayscale.cols * 0.01;
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

        prevCols = grayscale.cols;
    }

    Mat binary;

    // rozmiar bloku do adaptywnej binaryzacji jako 10% szerokości ramki
    int blockSize = 0.10 * grayscale.cols;
    if (blockSize % 2 == 0)
        blockSize++; // rozmiar bloku musi być nieparzysty

    // adaptacyjna binaryzacja
    //blur(binary, binary, Size(7, 7));
    adaptiveThreshold(grayscale, binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, blockSize, 0);

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

void Processor::calibrate(const std::vector<Marker>& markers) {
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
        p.push_back(Point(markers[i].x, markers[i].y));

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

std::vector<Marker> Processor::findMarkers(const Mat& binary, const Mat& grayscale) {
    using std::vector;
    vector<Marker> markers;

    // 1. znajdź kontury obiektów:

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(binary, contours, hierarchy,
                 CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_KCOS);

    if (!contours.size()) // avoid sigsegv ;)
        return markers;

    //drawContours(binary, contours, -1, Scalar(255, 255, 255));

    // 2. compare contours to our knowledge base (each-each)

    // porównujemy tylko zewnętrzne kontury, ponieważ
    // binaryzacja adaptacyjna zżera środki dużych figur;
    // nie da się dobrać odpowiedniego bloku do niej, żeby
    // nie znikał trójkąt, a jednocześnie nie robiła się dziura
    // w dużym zatkanym trójkącie
    // dlatego badamy kształty zewnętrzne oraz jasność piksela na
    // samym środku kształtu; jeśli jest jaśniejszy od średniej jasności
    // kształtu, znaczy to, że kształ jest pusty; w przeciwnym razie pełny
    // dla pełnych kształtów central/mean = ok. 94%
    // dla pustych kształtów central/mean = ok. 110%
    // więc działa i to całkiem nieźle ;)
    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
        // jeśli pole konturu < 5%^2 pola ramki, nie uwzględniaj go jako markera
        // ... czyli ostateczna rozgrywka ze "śmieciami"
        // to samo gdy > 30%^2 (ręka?)
        double area = contourArea(contours[idx]);
        if (area < 0.05 * 0.05 * frameArea || area > 0.30 * 0.30 * frameArea)
            continue;

        // porównaj z każdym elementem z bazy wiedzy
        double bestMatch = INFINITY;
        int bestI = -1;
        for (size_t i = 0; i < knownContours.size(); i++) {
            double match = matchShapes(contours[idx], knownContours[i].contour, CV_CONTOURS_MATCH_I1, 0);
            if (bestI == -1 || match < bestMatch) {
                bestI = i;
                bestMatch = match;
            }
        }

        // odrzuć na pewno błędne matche
        // to jest niesamowity odśmiecający trick, hell yeah
        if (bestMatch > 0.3)
            continue;

        // create the Marker
        Marker marker;
        marker.name = knownContours[bestI].name;

        // get the center of gravity rather than center of bounding box
        // (especially important in triangle case)
        Moments moms = moments(contours[idx]);
        double realX = (moms.m10 / moms.m00);
        double realY = (moms.m01 / moms.m00);
        marker.x = realX / (frameCenter.x * 2); // get relative (x,y)
        marker.y = realY / (frameCenter.y * 2);

        // sprawdź średni kolor w obrębie konturu i jak odbiega od niego kolor środka ciężkości
        Mat mask = Mat::zeros(grayscale.rows, grayscale.cols, CV_8UC1);
        drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
        double avg = mean(grayscale, mask)(0);
        double central = grayscale.at<uchar>(realY, realX);
        if (central > avg) // znaczy, że marker w środku pusty
            marker.name = "e." + marker.name;

        // enclosing circle
        cv::Point2f enclCircleCenter;
        float enclCircleRadius;
        minEnclosingCircle(contours[idx], enclCircleCenter, enclCircleRadius);
        marker.radius = enclCircleRadius;

        // push it to current frame marker vector
        markers.push_back(marker);
    }

    return markers;
}

void Processor::drawMarker(cv::Mat& canvas, const Marker& marker) {
    // important: colors are BGR, not RGB!

    Scalar color(255, 255, 255); // domyślnie biały

    if (marker.name == "cross" || marker.name == "e.cross")
        color = Scalar(0, 0, 255); // red
    else if (marker.name == "circle")
        color = Scalar(0, 255, 255); // yellow
    else if (marker.name == "e.circle")
        color = Scalar(0, 255, 0); // green
    else if (marker.name == "triangle")
        color = Scalar(255, 255, 0); // cyan
    else if (marker.name == "e.triangle")
        color = Scalar(255, 0, 0); // blue
    else if (marker.name == "rectangle")
        color = Scalar(255, 0, 255); // magenta
    else if (marker.name == "e.rectangle")
        color = Scalar(128, 0, 128); // purple

    double rx = marker.x * frameCenter.x * 2;
    double ry = marker.y * frameCenter.y * 2;

    // enclosing circle
    circle(canvas, cv::Point2d(rx, ry), marker.radius, color, 1, CV_AA);

    // center
    circle(canvas, cv::Point2d(rx, ry), 3, color, CV_FILLED, CV_AA);

    // label
    putText(canvas, marker.name, Point(rx + 6, ry + 3), FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1, CV_AA);
}

void Processor::buildKBContours() {
    knownContours.push_back(KnownContour("circle",    ":/resource/circle.png"));
    knownContours.push_back(KnownContour("cross",     ":/resource/cross.png"));
    knownContours.push_back(KnownContour("triangle",  ":/resource/triangle.png"));
    knownContours.push_back(KnownContour("triangle",  ":/resource/triangle_rounded.png"));

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
