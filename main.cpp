#include "CWidget.h"

#include <QApplication>
#include <QDebug>
#include <QScreen>
#include <QFile>
#include <QString>

// #include "qaLog.h"

void GetDeviceScreenDPI();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GetDeviceScreenDPI();

    QScreen *screen = a.primaryScreen();
    QSizeF devScrPhySize = screen->physicalSize();
    qreal devPixelRatio = screen->devicePixelRatio();
    QSizeF scrSize = screen->size();
    qreal xdpi = screen->physicalDotsPerInchX();
    qreal ydpi = screen->physicalDotsPerInchY();
    qreal scrdpi = screen->physicalDotsPerInch();
    qreal lMargin = 0, tMargin = 0;
    devScrType scrType = Longer;

#ifdef ANDROID
    qDebug() << "android device physical size: " << devScrPhySize << " mm." << endl;
    qDebug() << "android device pixel ratio: " << devPixelRatio << endl;
    qDebug() << "android device screen size: " << scrSize << " pixel." << endl;
    qDebug() << "android device screen physical x dpi: " << xdpi << endl;
    qDebug() << "android device screen physical y dpi: " << ydpi << endl;
    qDebug() << "android device screen physical dpi: " << scrdpi << endl;

    qreal devLWRatio = devScrPhySize.height()/devScrPhySize.width();
    scrType = (devLWRatio > gIdealLWRatio ? Longer : Wider);
    qreal h0 = devScrPhySize.height();
    qreal w0 = devScrPhySize.width();
    if(scrType == Wider)
    {        
        lMargin = (w0 - h0 * gIdealX / gIdealY) * xdpi / 10 / 2.54 / 2;
        qDebug() << "screen type is wider: lMargin = " << lMargin << endl;
    }
    else
    {
        tMargin = (h0 - w0 * gIdealY / gIdealX) * ydpi / 10 / 2.54 / 2;
        qDebug() << "screen type is longer: tMargin = " << tMargin << endl;
    }

    /*QA_LOG::logInit("/data/MiniGames/deviceInfo.txt");
    QString filename("/data/MiniGames/deviceInfo.txt");
    QFile file(filename);
    bool bOn = file.open(QFile::ReadWrite);
    if(!bOn)
    {
        qDebug() << "open file " << filename << " failed.\n" << endl;
    }
    else
    {
        file.write("log data");
        file.close();
    }*/
#else
    qDebug() << "desktop device physical size: " << devScrPhySize << " mm." << endl;
    qDebug() << "desktop device pixel ratio: " << devPixelRatio << endl;
    qDebug() << "desktop device screen size: " << scrSize << " pixel." << endl;
    qDebug() << "desktop device screen physical x dpi: " << xdpi << endl;
    qDebug() << "desktop device screen physical y dpi: " << ydpi << endl;
    qDebug() << "desktop device screen physical dpi: " << scrdpi << endl;

    scrSize = QSizeF(gIdealX, gIdealY)*.4;

    qreal devLWRatio = scrSize.height()/scrSize.width();
    scrType = (devLWRatio > gIdealLWRatio ? Longer : Wider);
    qreal h0 = scrSize.height();
    qreal w0 = scrSize.width();
    if(scrType == Wider)
    {
        lMargin = (w0 - h0 * gIdealX / gIdealY) / 2;
        qDebug() << "screen type is wider: lMargin = " << lMargin << endl;
    }
    else
    {
        tMargin = (h0 - w0 * gIdealY / gIdealX) / 2;
        qDebug() << "screen type is longer: tMargin = " << tMargin << endl;
    }
#endif

    CWidget w(int(scrSize.width()), int(scrSize.height()), scrType, lMargin, tMargin);
    w.setWindowFlags(Qt::FramelessWindowHint);
#ifdef ANDROID
    w.showFullScreen();
#else
    w.show();
#endif
    return a.exec();
}


/*
 * GetDeviceScreenDPI
 * refer to https://www.jianshu.com/p/f9e216c70ef9
*/
void GetDeviceScreenDPI()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenRect = screen->geometry();
    QSizeF physicalSize = screen->physicalSize();		// 屏幕物理尺寸，单位:毫米

    //设备无关像素值与像素的单位比值
    qreal devicePixelRatio = screen->devicePixelRatio();

    //设备无关像素值宽度。在手机设备上，这里得到的是设备无关像素值
    //(如：在1080x1920像素分辨率的手机上，这里得到的宽高可能类似于393x698这样的设备无关像素值，宽度为393)
    int screenW = screenRect.width();
    int screenH = screenRect.height();//设备无关像素值高度

    //手机屏幕真正的像素分辨率宽度, 高度
    int screenResolutionWidth = int(screenW*devicePixelRatio);
    int screenResolutionHeight= int(screenH*devicePixelRatio);

    //手机屏幕英寸宽度, 高度
    qreal physicalScreenWidthInch = qreal(physicalSize.width())/10*0.3937008;
    qreal physicalScreenHeightInch = qreal(physicalSize.height())/10*0.3937008;

    //像素宽度除以英寸宽度=像素密度, 高度
    int dpiW = int(screenResolutionWidth/physicalScreenWidthInch);
    int dpiH = int(screenResolutionHeight/physicalScreenHeightInch);

    qDebug() << "device screen rect: " << screenRect << endl;
    qDebug() << "device physical size: " << physicalSize << " mm" << endl;
    qDebug() << "device pixel ratio: " << devicePixelRatio << endl;
    qDebug() << "device screen physical x dpi: " << screen->physicalDotsPerInchX() << endl;
    qDebug() << "device screen pyysical y dpi: " << screen->physicalDotsPerInchY() << endl;
    qDebug() << "device screen logical x dpi: " << screen->logicalDotsPerInchX() << endl;
    qDebug() << "device screen logical y dpi: " << screen->logicalDotsPerInchY() << endl;

    qDebug() << "device resolution width: " << screenResolutionWidth << endl;
    qDebug() << "device resolution height: " << screenResolutionHeight << endl;    
    qDebug() << "physical screen width inch: " << physicalScreenWidthInch << endl;
    qDebug() << "physical screen height inch: " << physicalScreenHeightInch << endl;
    qDebug() << "pixel density width: " << dpiW << endl;
    qDebug() << "pixel density height: " << dpiH << endl;    
}




