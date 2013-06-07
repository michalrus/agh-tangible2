#ifndef CONTOUR_H
#define CONTOUR_H

#include <opencv2/opencv.hpp>

class Contour
{
public:
    /**
     * @brief Marker buduje konkretny marker
     * @param name user-friendly nazwa dla danego kształtu
     * @param resource ścieżka do pliku .png, który zawiera wzór kształtu
     */
    Contour(std::string name, std::string resource);

    std::string name;
    std::vector<cv::Point> contour;

private:
    /**
     * @brief imageFromResources wczytuje obrazek z zasobów Qt
     * @param path ścieżka do obrazka
     * @return macierz z obrazkiem
     */
    cv::Mat imageFromResources(const std::string path);
};

#endif // CONTOUR_H
