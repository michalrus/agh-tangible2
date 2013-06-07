#ifndef SYSTEMCONTROL_H
#define SYSTEMCONTROL_H

class SystemControl
{
public:
    /**
     * @brief setMouseXY Sets mouse position to (desktopWidth*x,desktopHeight*y).
     * @param x range: [0;1]
     * @param y range: [0;1]
     */
    void setMouseXY(double x, double y);

    void sendLeftArrow();
    void sendRightArrow();
};

#endif // SYSTEMCONTROL_H
