#include "CCell.h"

CCell::CCell()
    : QRect ()
    , m_bIsFilled(false)
    , m_bgrClr(QColor(0,0,0))
{

}

CCell::CCell(int left, int top, int width, int height, bool bFilled, QColor color)
    : QRect(left, top, width, height)
    , m_bIsFilled(bFilled)
    , m_bgrClr(color)
{

}

CCell &CCell::operator=(const CCell &cell)
{
    if(this != &cell)
    {
        this->setX(cell.x());
        this->setY(cell.y());
        this->setWidth(cell.width());
        this->setHeight(cell.height());

        this->m_bIsFilled = cell.m_bIsFilled;
        this->m_bgrClr = cell.m_bgrClr;
    }

    return *this;
}

QColor CCell::GetColor() const
{
    return m_bgrClr;
}

void CCell::SetColor(const QColor color)
{
    m_bgrClr = color;
}

bool CCell::IsFilled() const
{
    return m_bIsFilled;
}

void CCell::FillCell(const bool bFilled)
{
    m_bIsFilled = bFilled;
}

CCell CCell::moved(QPoint pt)
{
    return CCell(this->x() + pt.x() * this->width(),
                 this->y() + pt.y() * this->height(),
                 this->width(),
                 this->height(),
                 this->m_bIsFilled,
                 this->m_bgrClr
                 );
}


