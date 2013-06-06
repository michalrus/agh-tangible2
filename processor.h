#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <opencv2/opencv.hpp>

/**
 * @brief Enum, który zwraca metoda Processor::process.
 * Mówi o tym, który gest został wykonany przez użytkownika.
 */
enum Gesture {
    GNone,
    GBack,
    GForward
};

/**
 * @brief Klasa, która zajmuje się przetwarzaniem (rozumieniem) kolejnych ramek.
 */
class Processor
{
public:
    Processor();

    /**
     * @brief process stara się zrozumieć kolejne ramki z kamery.
     * @param frame Ramka obrazu z kamery. Wewnętrzny format OpenCV.
     * @return Zwraca gest użytkownika, jeśli wykryty. Jeśli nie: Gesture.None.
     */
    Gesture process(const cv::Mat& frame);

    /**
     * @brief getDebug
     * @return obrazek poglądowy (po jakichś przetworzeniach itd.) rysowany w oknie degug
     */
    const cv::Mat& getDebug() const;

    /**
     * @brief doCalibrate jest wywoływane przez GUI, gdy następna ramka ma zostać użyta do kalibracji.
     */
    void doCalibrate();

private:
    /**
     * @brief binarize przeprowadza binaryzację ramki (pod kątem zostawienia tylko markerów)
     * @param frame ramka z obrazem
     * @return zbinaryzowany obraz
     */
    cv::Mat binarize(const cv::Mat& frame);

    /**
     * @brief calibrate dokonuje kalibracji na podanej ramce
     * @param frame ramka używana do kalibracji
     */
    void calibrate(const cv::Mat& frame);

    /**
     * @brief findMarkers odszukuje markery na zbinaryzowanym obrazku
     * @param binary ramka zbinaryzowanego obrazu
     */
    void findMarkers(const cv::Mat& binary);

    /**
     * @brief calibrating mówi czy w następnej ramce kalibrujemy
     */
    bool calibrating;

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
};

#endif // PROCESSOR_H
