#ifndef GESTUREDETECTOR_H
#define GESTUREDETECTOR_H

#include <string>

class GestureDetector
{
public:
    GestureDetector();

    /**
     * @brief handle Funkcja obsługuje wystąpienie markera na stole w każdej ramce
     * @param type Typ markera (circle/triangle/e.circle itd.).
     * @param x Relatywna pozycja w szerokości stolika, zakres: [0;1]
     * @param y Relatywna pozycja w wysokości stolika, zakres: [0;1]
     */
    void handle(std::string type, double x, double y);

private:

    void back();
    void forward();
};

#endif // GESTUREDETECTOR_H
