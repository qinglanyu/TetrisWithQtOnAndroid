#ifndef CJBTN_H
#define CJBTN_H

#include <QPushButton>
class QTimer;

class CJBtn : public QPushButton
{
    Q_OBJECT
public:
    CJBtn(QWidget* parent);
    bool event(QEvent *e) override;
    void setStyleImg(const QString& normalImg, const QString& pressedImg);
    void setRepeat(const bool bRepeat = false);

signals:
    void sigVibrate(int);

private slots:
    void slotTimeOut();

private:
    QTimer *m_pTimer;
    int m_repeatInterval;
    int m_repeatDelay;
    bool m_repeat;
    QString m_szNormalImg;
    QString m_szPressedImg;
};

#endif // CJBTN_H
