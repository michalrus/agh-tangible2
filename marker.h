#ifndef MARKER_H
#define MARKER_H

#include <opencv2/opencv.hpp>

class Marker
{
public:
    Marker(std::string name, std::vector<cv::Point> contour);

    /**
     * @brief drawOn rysuje informacje o tym markerze
     * @param canvas obrazek na którym rysuje
     */
    void drawOn(cv::Mat& canvas);

    cv::Point getCenter() const;

private:
    /**
     * @brief getColor kolor w zależności od nazwy kształtu
     * @return (r,g,b)
     */
    cv::Scalar getColor() const;

    std::string name;
    std::vector<cv::Point> contour;

    cv::Moments moms;
    cv::Point gravityCenter;

    cv::Point2f enclCircleCenter;
    float enclCircleRadius;
};

#endif // MARKER_H
