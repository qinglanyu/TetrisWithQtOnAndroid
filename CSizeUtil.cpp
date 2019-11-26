#include "CSizeUtil.h"

#include <QApplication>
#include <QScreen>
#include <QFont>
#include <QString>
#include <QFontMetrics>



CSizeUtil &CSizeUtil::instance()
{
    static CSizeUtil sizeUtil;
    return sizeUtil;
}

int CSizeUtil::defaultFontHeight()
{
    return qApp->fontMetrics().height();
}

int CSizeUtil::widthWithDefaultFont(const QString &text)
{
    return qApp->fontMetrics().boundingRect(text).width();
}

int CSizeUtil::widthWithFont(const QString &text, int fontPointSize)
{
    QFont f = qApp->font();
    f.setPointSize(fontPointSize);
    QFontMetrics fm(f);
    return fm.boundingRect(text).width();
}

int CSizeUtil::fontHeight(int fontPointSize)
{
    QFont f = qApp->font();
    f.setPointSize(fontPointSize);
    QFontMetrics fm(f);
    return fm.height();
}

double CSizeUtil::dpiFactor()
{
    QScreen *screen = qApp->primaryScreen();
    return double(72/screen->logicalDotsPerInch());
}
