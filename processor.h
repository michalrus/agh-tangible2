#ifndef PROCESSOR_H
#define PROCESSOR_H

#include<opencv/cv.h>
#include<opencv/highgui.h>

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
     * @brief calibrate dokonuje kalibracji na podanej ramce
     * @param frame ramka używana do kalibracji
     */
    void calibrate(const cv::Mat& frame);

    /**
     * @brief calibrating mówi czy w następnej ramce kalibrujemy
     */
    bool calibrating;
    cv::Mat debug;
};

#endif // PROCESSOR_H