#include "marker.h"

using namespace cv;

Marker::Marker(std::string name_, std::vector<cv::Point> contour_)
    : name(name_),
      contour(contour_)
{
    // enclosing circle
    minEnclosingCircle(contour, enclCircleCenter, enclCircleRadius);

    // get the center of gravity rather than center of bounding box
    // (especially important in triangle case)
    moms = moments(contour);
    gravityCenter = Point(moms.m10 / moms.m00, moms.m01 / moms.m00);
}

void Marker::drawOn(cv::Mat &canvas) {
    Scalar color = getColor();

    // enclosing circle
    circle(canvas, enclCircleCenter, enclCircleRadius, color, 1, CV_AA);

    // center
    circle(canvas, gravityCenter, 3, color, CV_FILLED, CV_AA);

    // label
    putText(canvas, name, Point(gravityCenter.x + 6, gravityCenter.y + 3), FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 1, CV_AA);
}

Scalar Marker::getColor() const {
    // important: colors are BGR, not RGB!

    if (name == "cross" || name == "e.cross")
        return Scalar(0, 0, 255); // red

    if (name == "circle")
        return Scalar(0, 255, 255); // yellow

    if (name == "e.circle")
        return Scalar(0, 255, 0); // green

    if (name == "triangle")
        return Scalar(255, 255, 0); // cyan

    if (name == "e.triangle")
        return Scalar(255, 0, 0); // blue

    if (name == "rectangle")
        return Scalar(255, 0, 255); // magenta

    if (name == "e.rectangle")
        return Scalar(128, 0, 128); // purple

    // domyślnie biały
    return Scalar(255, 255, 255);
}
