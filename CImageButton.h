#ifndef CIMAGEBUTTON_H
#define CIMAGEBUTTON_H

#include <QPushButton>

class QPixmap;
class QTimer;
class CImageButton : public QPushButton
{
    Q_OBJECT

public:
    CImageButton(QWidget *parent = nullptr);
    CImageButton(const QString& text, QWidget *parent = nullptr);

    void SetPixmap(QPixmap* pNormal, QPixmap* pHover, QPixmap* pLighted);
    void SetLighted(bool bLighted);

public:
    bool event(QEvent *e) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;
    void setLongTouch(const bool bLong = true);
    void setHighLight(const bool bHlight = true);
    bool getLongTouch(void) const;
    void setFaceColor(const QColor& color);
    void setRepeatDelayInterval(const int nMsInterval=50, const int nMsDelay=200);
    void setAcceptTouch(const bool bAccept = true);

protected:
    void drawPicture(QPainter *painter);
    void drawElement(QPainter *painter);
    void drawFace(QPainter *painter);

private slots:
    void slotTimeOut();

signals:
    void sigVibrate(int ms);

protected:
    bool m_bAcceptTouch;
    bool m_bLighted;
    bool m_bMouseIn;
    bool m_bLeftDown;
    bool m_bLongTouch;
    bool m_highLight;
    int m_repeatDelay;
    int m_repeatInterval;
    QColor m_faceColor;

    QPixmap *m_pHover;
    QPixmap *m_pNormal;
    QPixmap *m_pLighted;

    QTimer *m_pTimer;
};

#endif // CIMAGEBUTTON_H
