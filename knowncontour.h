#ifndef KNOWNCONTOUR_H
#define KNOWNCONTOUR_H

#include <opencv2/opencv.hpp>

class KnownContour
{
public:
    /**
     * @brief Marker buduje konkretny marker
     * @param name user-friendly nazwa dla danego kształtu
     * @param resource ścieżka do pliku .png, który zawiera wzór kształtu
     */
    KnownContour(std::string name, std::string resource);

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

#endif // KNOWNCONTOUR_H
