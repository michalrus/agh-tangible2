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
     * @param frameNumber numer klatki od początku działania programu
     * @param type Typ markera (circle/triangle/e.circle itd.).
     * @param x Relatywna pozycja w szerokości stolika, zakres: [0;1]
     * @param y Relatywna pozycja w wysokości stolika, zakres: [0;1]
     */
    void handle(size_t frameNumber, std::string type, double x, double y);

private:
    /**
     * @brief ctl do wysyłania komend systemowych (strzałki, klawisze, myszka, etc.)
     */
    SystemControl systemControl;
};

#endif // GESTUREDETECTOR_H
