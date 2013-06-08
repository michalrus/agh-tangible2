#ifndef SYSTEMCONTROL_H
#define SYSTEMCONTROL_H

class QString;

class SystemControl
{
public:
    /**
     * @brief getCurrentWindowTitle retrieves a title of a current foreground window (globally)
     * @return the title
     */
    QString getCurrentWindowTitle() const;

    /**
     * @brief setMouseXY Sets mouse position to (desktopWidth*x,desktopHeight*y).
     * @param x range: [0;1]
     * @param y range: [0;1]
     */
    void sendMouseXY(double x, double y);

    /**
     * @brief sendMouseScroll scrolls using mouse wheel
     * @param velocity of scrolling, range: [-1;1] (positive means up)
     */
    void sendMouseScroll(double velocity);

    void sendLeftArrow();
    void sendRightArrow();
};

#endif // SYSTEMCONTROL_H
