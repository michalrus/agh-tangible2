#ifndef GESTUREDETECTOR_H
#define GESTUREDETECTOR_H

#include <string>

#include "systemcontrol.h"

class GestureDetector
{
public:
    GestureDetector();

    /**
     * @brief handle Funkcja obsługuje wystąpienie markera na stole w każdej ramce
     * @param control mówi czy chcemy w tej klatce kontrolować system
     * @param frameNumber numer klatki od początku działania programu
     * @param type Typ markera (circle/triangle/e.circle itd.).
     * @param x Relatywna pozycja w szerokości stolika, zakres: [0;1]
     * @param y Relatywna pozycja w wysokości stolika, zakres: [0;1]
     */
    void handle(bool control, size_t frameNumber, std::string type, double x, double y);

private:
    /**
     * @brief ctl do wysyłania komend systemowych (strzałki, klawisze, myszka, etc.)
     */
    SystemControl systemControl;

    /**
     * @brief lastClickFrame numer ramki w której ostatnio klikaliśmy
     */
    size_t lastClickFrame;
};

#endif // GESTUREDETECTOR_H
