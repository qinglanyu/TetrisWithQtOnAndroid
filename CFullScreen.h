#ifndef CFULLSCREEN_H
#define CFULLSCREEN_H

#ifdef ANDROID

#include <QAndroidJniObject>

class CFullScreen
{
public:
    CFullScreen();
    void fullScreenLeanBack();
    void fullScreenImmersive();
    void fullScreenStickyImmersive();

private:
    QAndroidJniObject m_javaClass;
};

#endif

#endif // CFULLSCREEN_H
