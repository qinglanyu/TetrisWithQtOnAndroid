#include "CFullScreen.h"

#ifdef ANDROID


#include <functional>
#include <QtAndroid>

#define JFS_CLASSNAME "CFullScreen"
#define JFS_SIGNTURE_INSTANCE "()Lorg/qtproject/CFullScreen"

void realFullScreenLeanBackMethod()
{
    QAndroidJniObject javaCustomFunction = QAndroidJniObject::callStaticObjectMethod(
                JFS_CLASSNAME, "instance", JFS_SIGNTURE_INSTANCE);
    javaCustomFunction.callMethod<void>("fullScreenLeanBack", "()V");
}
void realFullScreenImmersiveMethod() {
    QAndroidJniObject javaCustomFunction = QAndroidJniObject::callStaticObjectMethod(
                JFS_CLASSNAME, "instance", JFS_SIGNTURE_INSTANCE);
    javaCustomFunction.callMethod<void>("fullScreenImmersive", "()V");
}
void realFullScreenStickyImmersiveMethod() {
    QAndroidJniObject javaCustomFunction = QAndroidJniObject::callStaticObjectMethod(
                JFS_CLASSNAME, "instance", JFS_SIGNTURE_INSTANCE);
    javaCustomFunction.callMethod<void>("fullScreenStickyImmersive", "()V");
}


CFullScreen::CFullScreen()
{
    if(!QAndroidJniObject::isClassAvailable(JFS_CLASSNAME))
    {
        qDebug("JFullScreen is unavailable.");
        return;
    }

    m_javaClass = QAndroidJniObject::callStaticObjectMethod(
                JFS_CLASSNAME,
                "instance",
                JFS_SIGNTURE_INSTANCE
                );
    if(!m_javaClass.isValid()) {
        qDebug(" JFullScreen instance is invalid.");
    }

    m_javaClass.callMethod<void>(
                "setContext",
                "(Landroid/content/Context;)V",
                QtAndroid::androidActivity().object<jobject>()
                );
}

void CFullScreen::fullScreenLeanBack()
{
    QtAndroid::Runnable runnable = realFullScreenLeanBackMethod;
    QtAndroid::runOnAndroidThread(runnable);
}

void CFullScreen::fullScreenImmersive()
{
    QtAndroid::Runnable runnable = realFullScreenImmersiveMethod;
    QtAndroid::runOnAndroidThread(runnable);
}

void CFullScreen::fullScreenStickyImmersive()
{
    QtAndroid::Runnable runnable = realFullScreenStickyImmersiveMethod;
    QtAndroid::runOnAndroidThread(runnable);
}


#endif
