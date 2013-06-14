#ifndef GESTUREDETECTOR_H
#define GESTUREDETECTOR_H

#include <string>
#include <vector>
#include <marker.h>

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
    void handle(bool control, const std::vector<Marker>& frame);

    const std::vector<Marker>& getMarkers() const;

private:
    /**
     * @brief detectGestures rozpoznaje gesty z "bezwładnościowych" markerów
     */
    void detectGestures();

    /**
     * @brief markers "lepsze" markery -- biorąc pod uwagę kilka ostatnich klatek (większa bezwładność)
     */
    std::vector<Marker> markers;

    /**
     * @brief ctl do wysyłania komend systemowych (strzałki, klawisze, myszka, etc.)
     */
    SystemControl systemControl;

    /**
     * @brief lastClickFrame numer ramki w której ostatnio klikaliśmy
     */
    size_t lastClickFrame;

    /**
     * @brief frameNumber ile klatek upłynęło od początku programu
     */
    size_t frameNumber;
};

#endif // GESTUREDETECTOR_H
