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
    Gesture process(IplImage* frame);

    /**
     * @brief getDebug
     * @return obrazek poglądowy (po jakichś przetworzeniach itd.) rysowany w oknie degug
     */
    IplImage* getDebug();

    /**
     * @brief doCalibrate jest wywoływane, gdy następna ramka ma zostać użyta do kalibracji.
     */
    void doCalibrate();

private:
    /**
     * @brief calibrating mówi czy w następnej ramce kalibrujemy
     */
    bool calibrating;
    IplImage* debug;
};

#endif // PROCESSOR_H
