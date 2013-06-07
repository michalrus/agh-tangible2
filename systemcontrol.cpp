#include "systemcontrol.h"

#include <QApplication>
#include <iostream>

#if defined(Q_OS_WIN)
#   define WINVER 0x0500
#   include <windows.h>
#else
#   error Unknown operating system, no specific SystemControl available.
#endif

void SystemControl::setMouseXY(double x, double y) {
#if defined Q_OS_WIN
    INPUT ip;
    ip.type = INPUT_MOUSE;
    ip.mi.dx = 65536 * x;
    ip.mi.dy = 65536 * y;
    ip.mi.mouseData = 0;
    ip.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    ip.mi.time = 0;
    ip.mi.dwExtraInfo = NULL;
    SendInput(1, &ip, sizeof(INPUT));
#endif
}

#if defined Q_OS_WIN
void SystemControl_sendWindowsKeybd(WORD wVk) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.wVk = wVk;
    // press
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));
    // release
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}
#endif

void SystemControl::sendLeftArrow() {
#if defined Q_OS_WIN
    SystemControl_sendWindowsKeybd(VK_LEFT);
#endif
}

void SystemControl::sendRightArrow() {
#if defined Q_OS_WIN
    SystemControl_sendWindowsKeybd(VK_RIGHT);
#endif
}
