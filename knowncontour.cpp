#include "knowncontour.h"

#include <QImage>
#include <QDebug>

//#include <opencv2/imgproc.hpp>
//#include <opencv2/imgproc/types_c.h>

using namespace cv;

KnownContour::KnownContour(std::string name_, std::string resource)
{
    name = name_;

    Mat binary = imageFromResources(resource);

    using std::vector;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(binary, contours, hierarchy,
                 CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_KCOS);

    if (contours.size() != 1) {
        qCritical() << QObject::tr("Template image ") + QString(resource.c_str()) + QObject::tr(" has more than one contour (or no at all!). Aborting.");
        exit(-1);
    }

    contour = contours[0];
}

cv::Mat KnownContour::imageFromResources(const std::string path) {
    QImage src(path.c_str());
    Mat dst = Mat(src.height(), src.width(), CV_8UC1, Scalar(0));
    for (int y = 0; y < src.height(); y++)
        for (int x = 0; x < src.width(); x++) {
            QRgb p = src.pixel(x, y);
            int r = qRed(p);
            int g = qGreen(p);
            int b = qBlue(p);
            if (r + g + b > 127 * 3) // if object (white)
                dst.at<uchar>(y,x) = 255;
        }

    return dst;
}
