#include "CImageButton.h"
#include "gameglobal.h"
#include "CSizeUtil.h"

#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QTouchEvent>
#include <QTimer>
#include <QBitmap>
#include <QDebug>
#include <math.h>

CImageButton::CImageButton(QWidget *parent)
    : QPushButton(parent)
    , m_bAcceptTouch(false)
    , m_bLighted(false)
    , m_bMouseIn(false)
    , m_bLeftDown(false)
    , m_bLongTouch(false)
    , m_highLight(false)
    , m_repeatDelay(200)
    , m_repeatInterval(50)
    , m_faceColor(btnClr)
    , m_pHover(nullptr)
    , m_pNormal(nullptr)
    , m_pLighted(nullptr)
    , m_pTimer(nullptr)
{
    setFocusPolicy(Qt::NoFocus);
    setAcceptTouch();

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
}

CImageButton::CImageButton(const QString &text, QWidget *parent)
    : CImageButton(parent)
{
    setText(text);
}

void CImageButton::SetPixmap(QPixmap *pNormal, QPixmap *pHover, QPixmap *pLighted)
{
    m_pNormal = pNormal;
    m_pHover = pHover;
    m_pLighted = pLighted;
    // setMask(QBitmap(pNormal->mask()));
}

void CImageButton::SetLighted(bool bLighted)
{
    if(bLighted != m_bLighted)
    {
        m_bLighted = bLighted;
        repaint();
    }
}

bool CImageButton::event(QEvent *e)
{
    if(m_bLongTouch)
    {
        QEvent::Type type = e->type();
        switch (type)
        {
        case QEvent::TouchBegin:
            e->accept();
            emit clicked();
            emit sigVibrate(30);
            m_bLeftDown = true;
            m_pTimer->setSingleShot(true);
            m_pTimer->start(m_repeatDelay);
            qDebug() << "ImageButton TouchBegin " << objectName() << endl;
            repaint();
            return true;
            break;

        case QEvent::TouchUpdate:
            e->accept();
            return true;

        case QEvent::TouchEnd:
            e->accept();
            if(m_pTimer->isActive())
                m_pTimer->stop();
            m_bLeftDown = false;
            qDebug() << "ImageButton TouchEnd " << objectName() << endl;
            repaint();
            return true;

        default:
            break;
        }
    }
    else if(e->type() == QEvent::TouchBegin)
    {
        emit sigVibrate(30);
    }
    else
    {

    }

    return QPushButton::event(e);
}

void CImageButton::paintEvent(QPaintEvent *e)
{
    QString ss = styleSheet();
    if(ss.isEmpty())
    {
        QPainter painter(this);
        drawPicture(&painter);
    }
    else
        return QPushButton::paintEvent(e);
}

void CImageButton::mousePressEvent(QMouseEvent *e)
{
    qDebug() << "mousePressEvent called " << endl;
    if(e->button() == Qt::LeftButton)
    {
        m_bLeftDown = true;
        repaint();
    }

    QPushButton::mousePressEvent(e);
}

void CImageButton::mouseReleaseEvent(QMouseEvent *e)
{
    qDebug() << "mouseReleaseEvent called " << endl;
    if(e->button() == Qt::LeftButton)
    {
        m_bLeftDown = false;
        repaint();
    }

    QPushButton::mouseReleaseEvent(e);
}

void CImageButton::enterEvent(QEvent *)
{
    qDebug() << "enterEvent called " << endl;
    m_bMouseIn = true;
    repaint();
}

void CImageButton::leaveEvent(QEvent *)
{
    qDebug() << "leaveEvent called " << endl;
    m_bMouseIn = false;
    repaint();
}

void CImageButton::setLongTouch(const bool bLong)
{
    m_bLongTouch = bLong;

#ifndef ANDROID
    if(m_bLongTouch)
    {
        setAutoRepeat(true);
        setAutoRepeatDelay(m_repeatDelay);
        setAutoRepeatInterval(m_repeatInterval);
    }
    else
        setAutoRepeat(false);
#endif
}

void CImageButton::setHighLight(const bool bHlight)
{
    m_highLight = bHlight;
}

bool CImageButton::getLongTouch() const
{
    return m_bLongTouch;
}

void CImageButton::setFaceColor(const QColor &color)
{
    m_faceColor = color;
}

void CImageButton::setRepeatDelayInterval(const int nMsInterval, const int nMsDelay)
{
    m_repeatDelay = nMsDelay;
    m_repeatInterval = nMsInterval;
}

void CImageButton::setAcceptTouch(const bool bAccept)
{
    m_bAcceptTouch = bAccept;
    setAttribute(Qt::WA_AcceptTouchEvents);
}

void CImageButton::drawPicture(QPainter *painter)
{
    if(!m_pNormal || !m_pHover || !m_pLighted)
        return;

    QPixmap *pPixmap = nullptr;
    if(m_bLighted || m_bLeftDown)
        pPixmap = m_pLighted;
    else
        pPixmap = (m_bMouseIn ? m_pHover : m_pNormal);

    if(pPixmap)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawPixmap(rect(), pPixmap->scaled(size(), Qt::KeepAspectRatioByExpanding));
    }
    if(text().count() > 0)
        painter->drawText(rect(), Qt::AlignCenter, text());
}

void CImageButton::drawElement(QPainter *painter)
{
    painter->translate(width() / 2, height() / 2);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::transparent);
    painter->fillRect(-width(), -height(), width() * 2, height() * 2, bgr);

    int radius = qMin(width(), height())/2;

    // 外边框
    QLinearGradient lg1(0, -radius, 0, radius);
    lg1.setColorAt(0, white);
    lg1.setColorAt(1, white1);
    painter->setBrush(lg1);
    painter->drawEllipse(-radius, -radius, radius << 1, radius << 1);

    // 内边框
    radius -= (radius/10);
    QLinearGradient lg2(0, -radius, 0, radius);
    lg2.setColorAt(0, white0);
    lg2.setColorAt(1, white);
    painter->setBrush(lg2);
    painter->drawEllipse(-radius, -radius, radius << 1, radius << 1);

    // 黑色衬底
    int bRadius = radius - (radius/10);
    bRadius = radius;
    QRadialGradient rg(0, 0, bRadius);
    rg.setColorAt(0, QColor(0, 0, 0, 128));
    rg.setColorAt(0.6, QColor(0, 0, 0, 128));
    rg.setColorAt(1, QColor(0, 0, 0, 128));
    painter->setBrush(rg);
    painter->drawEllipse(-bRadius, -bRadius, bRadius << 1, bRadius << 1);

    // 内部的圆
    QColor blue(0, 0xAE, 0xF0);
    blue = QColor(0x00, 0x9F, 0xF6);
    radius -= (m_bLeftDown? (radius/15) : 3);
    QRadialGradient rg1(0, 0, radius);
    rg1.setColorAt(0, blue);
    rg1.setColorAt(1, blue);
    painter->setBrush(rg1);
    painter->drawEllipse(-radius, -radius, radius << 1, radius << 1);

    // 高光
    radius = int(radius * 0.8);
    QPainterPath path;
    path.addEllipse(-radius, -radius, radius << 1, radius << 1);

    QPainterPath bigEllipse;
    bigEllipse.addEllipse(-radius, -radius/2, radius << 1, radius << 1);
    path -= bigEllipse;

    QLinearGradient lg3(0, -radius / 2, 0, 0);
    lg3.setColorAt(0, QColor(255, 255, 255, 120));
    lg3.setColorAt(0.2, QColor(255, 255, 255, 80));
    lg3.setColorAt(0.4, QColor(255, 255, 255, 60));
    lg3.setColorAt(0.6, QColor(0x00, 0x9F, 0xF6, 191));
    lg3.setColorAt(0.8, QColor(0x00, 0x9F, 0xF6, 191));
    lg3.setColorAt(1, QColor(0x00, 0x9F, 0xF6, 191));

    painter->setBrush(lg3);
    painter->drawPath(path);
}

void CImageButton::drawFace(QPainter *painter)
{
    QPainterPath path0;
    path0.addEllipse(rect());
    painter->fillPath(path0, QBrush(m_faceColor));

    qreal pressedFactor = .95;
    qreal hlcFactor = .6;       // high-light circle factor
    qreal rad = width()/2;
    QRadialGradient gr(rad, rad, rad, rad, rad);
    if(m_bLeftDown)
    {
        gr.setColorAt(0.0, m_faceColor);
        gr.setColorAt(.81*pressedFactor, m_faceColor);       // circle
    }
    else
    {
        gr.setColorAt(0.0, m_faceColor);
        gr.setColorAt(.83, m_faceColor);         // circle
    }
    gr.setColorAt(.86, Qt::black);
    gr.setColorAt(.89, bgr);
    gr.setColorAt(.90, QColor("#A97F0C"));
    gr.setColorAt(.98, bgr);
    gr.setColorAt(1, bgr);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(gr);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(rect());

    if(m_highLight)
    {
        painter->translate(rect().center());
        painter->setPen(Qt::transparent);
        qreal radius = rad*hlcFactor;
        if(m_bLeftDown) radius *= pressedFactor;
        QRectF arcRt(-radius, -radius, radius*2, radius*2);
        QPainterPath bezier;
        bezier.moveTo(radius, 0);
        bezier.arcTo(arcRt, 0, 180);
        bezier.moveTo(radius, 0);
        QPointF pt0(radius*.8, radius*.2), pt1(-radius*.1, -radius*1.2), pend(-radius, 0);
        bezier.cubicTo(pt0, pt1, pend);

        QLinearGradient lg(QPointF(width()/2*.85, -height()/2), QPointF(width()/2*.15,height()/2));
        lg.setColorAt(0, QColor(255, 255, 255, 180));
        lg.setColorAt(1, m_faceColor);
        painter->fillPath(bezier, QBrush(lg));
    }
}

void CImageButton::slotTimeOut()
{
    m_pTimer->start(m_repeatInterval);

    emit clicked();
}
