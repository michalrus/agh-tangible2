#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <opencv2/opencv.hpp>

#include "gesturedetector.h"
#include "contour.h"
#include "marker.h"

/**
 * @brief Klasa, która zajmuje się przetwarzaniem (rozumieniem) kolejnych ramek.
 */
class Processor
{
public:
    Processor();

    /**
     * @brief process wykrywa markery na kolejnych ramkach z kamery.
     * @param frame Ramka obrazu z kamery. Wewnętrzny format OpenCV.
     */
    void process(const cv::Mat& frame);

    /**
     * @brief getDebug
     * @return obrazek poglądowy (po jakichś przetworzeniach itd.) rysowany w oknie degug
     */
    const cv::Mat& getDebug() const;

    /**
     * @brief doCalibrate jest wywoływane przez GUI, gdy następna ramka ma zostać użyta do kalibracji.
     */
    void doCalibrate();

    /**
     * @brief doReset jest wywoływane przez GIU, gdy w następnej ramce ma zostać zresetowana macierz kalibracji. (Czyli jak gdyby kalibracja nigdy nie została przeprowadzona).
     */
    void doReset();

private:
    /**
     * @brief binarize przeprowadza binaryzację ramki (pod kątem zostawienia tylko markerów)
     * @param frame ramka z obrazem
     * @return zbinaryzowany obraz
     */
    cv::Mat binarize(const cv::Mat& frame);

    /**
     * @brief calibrate dokonuje kalibracji na rozpoznanych Marker-ach
     */
    void calibrate();

    /**
     * @brief findMarkers odszukuje markery na zbinaryzowanym obrazku
     * @param binary ramka zbinaryzowanego obrazu
     */
    void findMarkers(const cv::Mat& binary);

    /**
     * @brief buildContours buduje wiedzę (wczytuje wzory kształtów)
     */
    void buildKBContours();

    /**
     * @brief gestureDetector obsługuje już rozpoznane pozycje+typy markerów
     */
    GestureDetector gestureDetector;

    /**
     * @brief calibrating mówi czy w następnej ramce kalibrujemy
     */
    bool calibrating;

    /**
     * @brief warping mówi czy na wszystkich ramkach robimy warpPerspective().
     */
    bool warping;

    /**
     * @brief warpPerspectiveTransformMatrix macierz trasformacji warpPerspective()
     */
    cv::Mat warpPerspectiveTransformMatrix;

    /**
     * @brief debug macierz z obrazkiem wyświetlanym jako debug w GUI
     */
    cv::Mat debug;

    /**
     * @brief strel element strukturalny do operacji otwarcia
     */
    cv::Mat strelOpen;

    /**
     * @brief strelOpen element strukturalny do operacji zamknięcia
     */
    cv::Mat strelClose;

    /**
     * @brief prevCols szerokość poprzedniej ramki (do odpowiedniego zmieniania strel)
     */
    int prevCols;

    /**
     * @brief frameArea pole powierzchni ramki
     */
    int frameArea;

    /**
     * @brief frameCenter punkt na środku ramki (potrzebny do sortowania punktów w kalibracji)
     */
    cv::Point frameCenter;

    /**
     * @brief framesPassed liczba klatek, które upłynęły od początku działania programu
     */
    size_t framesPassed;

    /**
     * @brief knownContours baza wiedzy (kontury kształty, które umiemy rozpoznać)
     */
    std::vector<Contour> knownContours;

    /**
     * @brief markers markery rozpoznane na obrazku
     */
    std::vector<Marker> markers;
};

#endif // PROCESSOR_H
