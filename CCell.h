#ifndef CCELL_H
#define CCELL_H

#include "gameglobal.h"

#include <QRect>
#include <QRectF>
#include <QColor>
#include <QPoint>

class CCell : public QRect
{
public:
    CCell();
    CCell(int left, int top, int width, int height, bool bFilled=false, QColor color=gameboardBgr);
    CCell &operator=(const CCell &cell);

    QColor GetColor() const;
    void SetColor(const QColor color);

    bool IsFilled() const;
    void FillCell(const bool bFilled = true);
    CCell moved(QPoint pt);


private:
    bool m_bIsFilled;
    QColor m_bgrClr;
};


#endif // CCELL_H
