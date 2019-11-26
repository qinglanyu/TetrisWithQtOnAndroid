#include "CJBtn.h"

#include <QEvent>
#include <QDebug>
#include <QTimer>

CJBtn::CJBtn(QWidget *parent)
    : QPushButton(parent)
    , m_pTimer(nullptr)
    , m_repeatInterval(50)
    , m_repeatDelay(500)
    , m_repeat(true)
{
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_AcceptTouchEvents);

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
}

bool CJBtn::event(QEvent *e)
{
    QEvent::Type type = e->type();
    switch (type)
    {
    case QEvent::TouchBegin:
        e->accept();
        qDebug() << "CJBtn TouchBegin " << objectName() << endl;
        m_pTimer->setSingleShot(true);
        m_pTimer->start(m_repeatDelay);
        emit sigVibrate(30);
        emit clicked();
        setStyleSheet(QString("CJBtn{border-image: url(%1);}").arg(m_szPressedImg));
        return true;

    case QEvent::TouchUpdate:
        e->accept();
        break;

    case QEvent::TouchEnd:
        e->accept();
        if(m_pTimer->isActive())
            m_pTimer->stop();
        qDebug() << "CJBtn TouchEnd " << objectName() << endl;
        setStyleSheet(QString("CJBtn{border-image: url(%1);}").arg(m_szNormalImg));
        return true;

    default:
        break;
    }
    return QPushButton::event(e);
}

void CJBtn::setStyleImg(const QString &normalImg, const QString &pressedImg)
{
    m_szNormalImg = normalImg;
    m_szPressedImg = pressedImg;
    setStyleSheet(QString("CJBtn{border-image: url(%1);}").arg(m_szNormalImg));
}

void CJBtn::setRepeat(const bool bRepeat)
{
    m_repeat = bRepeat;
}

void CJBtn::slotTimeOut()
{
    m_pTimer->start(m_repeatInterval);
    if(m_repeat)
        emit clicked();
}
