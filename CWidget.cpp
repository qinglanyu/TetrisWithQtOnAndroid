#include "CWidget.h"
#include "gameglobal.h"
#include "CJBtn.h"

#include <QLabel>
#include <QMouseEvent>
#include <QTimer>
#include <QRect>
#include <QLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QDebug>
#include <QPixmap>
#include <QEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontDatabase>
#include <QMediaPlayer>
#include <QKeyEvent>

#ifdef ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>

using namespace QtAndroid;

#define CHECK_EXCEPTION() \
    if(env->ExceptionCheck())\
    {\
        qDebug() << "exception occured";\
        env->ExceptionDescribe();\
        env->ExceptionClear();\
    }
#endif

#define NELEMENTS(x) (sizeof(x)/sizeof(x[0]))

#include "CSizeUtil.h"
#include "CGameManager.h"
#include "CCell.h"
#include "./Games/Tetris/CTetris.h"


CWidget::CWidget(int iWidth, int iHeight, devScrType scrtype, qreal lmargin, qreal tmargin, QWidget *parent)
    : QWidget(parent)
    , m_width(iWidth)
    , m_height(iHeight)
    , m_scrType(scrtype)
    , m_lMargin(lmargin)
    , m_tMargin(tmargin)
    , m_devXFactor(1.0)
    , m_devYFactor(1.0)
    , m_bVoiceOn(true)
    , m_bPaused(false)
    , m_bVibrateOn(true)
    , m_player(nullptr)
    , m_pUpBtn(nullptr)
    , m_pDownBtn(nullptr)
    , m_pLeftBtn(nullptr)
    , m_pRightBtn(nullptr)
    , m_pRotateBtn(nullptr)
    , m_pStartBtn(nullptr)
    , m_pSoundBtn(nullptr)
    , m_pSettingBtn(nullptr)
    , m_pExitBtn(nullptr)
    , m_pGameMgr(nullptr)
    , m_pxbgr(nullptr)
    , m_pxfgr(nullptr)
    , m_pxscr(nullptr)
{
    this->setFixedSize(m_width, m_height);
    qDebug() << "widget size: " << this->size() << endl;

    // init game manager
    m_pGameMgr = new CGameManager;
    m_player = new QMediaPlayer(this);

    InitUI();

    InitConnectors();

    // initialize the element position first only once
    InitPositon();

    DrawBackground();
}

CWidget::~CWidget()
{
    if(m_pGameMgr != nullptr)
    {
        delete m_pGameMgr;
        m_pGameMgr = nullptr;
    }

    if(m_pxbgr != nullptr)
    {
        delete m_pxbgr;
        m_pxbgr = nullptr;
    }

    if(m_pxfgr != nullptr)
    {
        delete m_pxfgr;
        m_pxfgr = nullptr;
    }

    if(m_pxscr != nullptr)
    {
        delete m_pxscr;
        m_pxscr = nullptr;
    }
}

void CWidget::InitUI()
{
    if(m_scrType == Wider)
    {
        m_devXFactor = qreal(m_width-m_lMargin*2) / gIdealX;
        m_devYFactor = qreal(m_height) / gIdealY;
    }
    else    // Longer
    {
        m_devXFactor = qreal(m_width) / gIdealX;
        m_devYFactor = qreal(m_height-m_tMargin*2) / gIdealY;
    }

    static QString fontRes = QString(":/localfont/font/DS-Digital/DS-DIGI-1.ttf");
    static int fontId = QFontDatabase::addApplicationFont(fontRes);
    static QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);

#ifdef ANDROID
    int fontPointSize = int(20 * m_devXFactor);
#else
    int fontPointSize = int(36 * m_devXFactor);
#endif
    m_digitFont = QFont(fontName, fontPointSize);

    InitGameBtns();
}

void CWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,*m_pxbgr);
    DrawGameText(painter);
    DrawGameBoard(painter);
    DrawMovingShape(painter);
    DrawPreviewBoard(painter);
}

void CWidget::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();

    switch (key)
    {
    case Qt::Key_Up:
    case Qt::Key_W:
        emit sigPushButton(CGameManager::MD_Up);
        break;

    case Qt::Key_Left:
    case Qt::Key_A:
        emit sigPushButton(CGameManager::MD_Left);
        break;

    case Qt::Key_Right:
    case Qt::Key_D:
        emit sigPushButton(CGameManager::MD_Right);
        break;

    case Qt::Key_Down:
    case Qt::Key_S:
        emit sigPushButton(CGameManager::MD_Down);
        break;

    case Qt::Key_Space:
        emit sigPushButton(CGameManager::MD_Floor);
        break;
    }

    QWidget::keyPressEvent(event);
}

void CWidget::paintCell(QPainter &painter, CCell &cell, bool moving)
{
    QRectF cellF(cell);
    QColor lineColor((moving || cell.IsFilled())? Qt::black : shadow);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(lineColor);
    painter.setPen(pen);
    painter.setBrush(lineColor);

    // draw cell boundry
    QPointF t0(cell.topLeft()), t1(cell.bottomLeft()), t2(cell.bottomRight()), t3(cell.topRight());
    QLineF lines[4] = { QLineF(t0, t1), QLineF(t1, t2), QLineF(t2, t3), QLineF(t3, t0) };
    painter.drawLines(lines, 4);

    // fill cell center
    QColor cellColor = ((moving || cell.IsFilled()) ? cell.GetColor() : shadow);
    qreal lMargin = cellF.width()*0.12;
    painter.fillRect(cellF.adjusted(lMargin,lMargin, -lMargin, -lMargin), cellColor);
}

void CWidget::DrawSinkFrame(QPainter &painter)
{
    qreal sinkFW = 12.0 * m_devXFactor;

    QPointF pt0(m_sinkFrame.topLeft()),
            pt1(m_sinkFrame.bottomLeft()),
            pt2(m_sinkFrame.bottomRight()),
            pt3(m_sinkFrame.topRight()),
            pt4(m_sinkFrame.topLeft()+QPointF(sinkFW, sinkFW)),
            pt5(m_sinkFrame.bottomLeft()+QPointF(sinkFW, -sinkFW)),
            pt6(m_sinkFrame.bottomRight()+QPointF(-sinkFW,-sinkFW)),
            pt7(m_sinkFrame.topRight()+QPointF(-sinkFW, sinkFW));

    // Lttob
    QLinearGradient grd;
    grd.setColorAt(0, bgr);
    grd.setColorAt(1, QColor("#665E0B"));
    painter.setRenderHint(QPainter::Antialiasing);
    grd.setStart(pt0);
    grd.setFinalStop(pt0+QPointF(sinkFW,0));
    QVector<QPointF> pts{pt0, pt1, pt5, pt4, pt0};
    QPolygonF polygon(pts);
    QPainterPath path0;
    path0.addPolygon(polygon);
    painter.fillPath(path0, QBrush(grd));

    // Tltor
    grd.setStart(pt4+QPointF(0,-sinkFW));
    grd.setFinalStop(pt4);
    pts.clear();
    pts << pt0 << pt4 << pt7 << pt3;
    QPolygonF polygon2(pts);
    QPainterPath path1;
    path1.addPolygon(polygon2);
    painter.fillPath(path1, QBrush(grd));

    // Rttob
    grd.setStart(pt7);
    grd.setFinalStop(pt7+QPointF(sinkFW,0));
    grd.setColorAt(0, QColor("#DCCC76"));
    grd.setColorAt(.3, QColor("#DCCC76"));
    grd.setColorAt(1, bgr);
    pts.clear();
    pts << pt7 << pt6 << pt2 << pt3 << pt7;
    QPolygonF polygon3(pts);
    QPainterPath path2;
    path2.addPolygon(polygon3);
    painter.fillPath(path2, QBrush(grd));

    // Bltor
    grd.setStart(pt5);
    grd.setFinalStop(pt5+QPointF(0,sinkFW));
    pts.clear();
    pts << pt5 << pt1 << pt2 << pt6 << pt5;
    QPolygonF polygon4(pts);
    QPainterPath path3;
    path3.addPolygon(polygon4);
    painter.fillPath(path3, QBrush(grd));

    QPen pen;
    pen.setWidthF(4);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    QRectF lineFrame = m_sinkFrame.adjusted(sinkFW, sinkFW, -sinkFW, -sinkFW);
    painter.fillRect(lineFrame, sinkBgr);

    painter.drawLine(lineFrame.topLeft(), lineFrame.bottomLeft());
    painter.drawLine(lineFrame.bottomLeft(), lineFrame.bottomRight());
    painter.drawLine(lineFrame.bottomRight(), lineFrame.topRight());
    painter.drawLine(lineFrame.topRight(), lineFrame.topLeft());

    m_updateRt = QRect(int(lineFrame.left()), int(lineFrame.top()), int(lineFrame.width()), int(lineFrame.height()));
}

void CWidget::DrawBtnCircles(QPainter &painter)
{
    painter.save();

    QPointF center;
    qreal radius;
    //const static qreal btnFactors[] = {.1, .12, .12, .12, .12, .2, .2, .2, .2};
    const static qreal btnFactors[] = {.12, .15, .15, .15, .15, .2, .2, .2, .2};

    QPushButton* pBtnTbl[] = {m_pRotateBtn, m_pUpBtn, m_pDownBtn, m_pLeftBtn, m_pRightBtn,
                               m_pStartBtn, m_pSoundBtn, m_pSettingBtn, m_pExitBtn};

    size_t cnt = sizeof(pBtnTbl)/sizeof(pBtnTbl[0]);
    for(size_t i=0; i<cnt; ++i)
    {
        if(pBtnTbl[i])
        {
            center = pBtnTbl[i]->frameGeometry().center();
            radius = pBtnTbl[i]->frameGeometry().width()/2;
            QPainterPath path;
            path.addEllipse(center, radius, radius);
            path.addEllipse(center, radius*(1+btnFactors[i]), radius*(1+btnFactors[i]));
            QRadialGradient gr(center, radius*(1+btnFactors[i]), center);
            gr.setColorAt(0, bgr);
            gr.setColorAt((1.00)/(1+btnFactors[i]), QColor("#A67D0C"));
            gr.setColorAt(1, bgr);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.fillPath(path, QBrush(gr));
        }
    }

    painter.restore();
}

void CWidget::DrawGameText(QPainter &painter)
{
    painter.setPen(Qt::NoPen);
    painter.setPen(Qt::black);    

    int nScore = (m_pGameMgr==nullptr? 0 : m_pGameMgr->GetScore());
    int nLevel = (m_pGameMgr==nullptr? 0 : m_pGameMgr->GetLevel());
    int nSpeed = (m_pGameMgr==nullptr? 0 : m_pGameMgr->GetSpeed());

    painter.setFont(m_digitFont);
    painter.drawText(m_scoreStrFrm, Qt::AlignCenter, "SCORE");
    painter.drawText(m_hiscrStrFrm, Qt::AlignCenter, "HI-SCORE");
    painter.drawText(m_scoreNumFrm, Qt::AlignCenter, QString::number(nScore));
    painter.drawText(m_hiscrNumFrm, Qt::AlignCenter, QString::number(nScore));
    painter.drawText(m_levelFrm, Qt::AlignCenter, "LEVEL  "+QString::number(nLevel));
    painter.drawText(m_speedFrm, Qt::AlignCenter, "SPEED  "+QString::number(nSpeed));
}

void CWidget::DrawStaticText(QPainter &painter)
{
    {
        QPixmap pixmap(m_bVoiceOn ? ":/img/img/sound.png" : ":/img/img/mute.png");
        qreal pxW = pixmap.width();
        qreal pxH = pixmap.height();
        qreal factor = m_soundFrm.height() / pxH;
        pixmap = pixmap.scaled(int(pxW*factor),int(pxH*factor),Qt::KeepAspectRatio);
        QPointF pt = m_soundFrm.topLeft() + QPointF((m_soundFrm.width()-pxW*factor)/2, 0);
        painter.drawPixmap(pt, pixmap);
    }

    {
        QPixmap pixmap(m_bPaused ? ":/img/img/paused.png" : ":/img/img/goon.png");
        qreal pxW = pixmap.width();
        qreal pxH = pixmap.height();
        qreal factor = m_pauseFrm.height() / pxH;
        pixmap = pixmap.scaled(int(pxW*factor),int(pxH*factor),Qt::KeepAspectRatio);
        QPointF pt = m_pauseFrm.topLeft() + QPointF((m_pauseFrm.width()-pxW*factor)/2*.2, 0);
        painter.drawPixmap(pt, pixmap);
    }

    // draw button tips
#ifdef ANDROID
    int fontPointSize = int(20 * m_devXFactor);
    int settingPtSz = int(12 * m_devXFactor);
#else
    int fontPointSize = int(40 * m_devXFactor);
    int settingPtSz = int(22 * m_devXFactor);
#endif
    QFont font("Arial", fontPointSize);
    font.setItalic(true);
    font.setPointSize(settingPtSz);
    font.setBold(true);
    painter.setFont(font);

    QRect btnRt = m_pStartBtn->frameGeometry();
    QPoint topLeft = btnRt.bottomLeft() + QPoint(-btnRt.width()/2, int(10*m_devYFactor));
    QRectF btnTipRect(topLeft, btnRt.size()*2);
    painter.drawText(btnTipRect, Qt::AlignTop|Qt::AlignHCenter, "Start\nPause");

    int nSpace = m_pSoundBtn->frameGeometry().x() - m_pStartBtn->frameGeometry().x();
    topLeft += QPoint(nSpace, 0);
    btnTipRect.moveTo(topLeft);
    painter.drawText(btnTipRect, Qt::AlignTop|Qt::AlignHCenter, "Sound\nVibrate");

    topLeft += QPoint(nSpace, 0);
    btnTipRect.moveTo(topLeft);
    painter.drawText(btnTipRect, Qt::AlignTop|Qt::AlignHCenter, "Setting");

    topLeft += QPoint(nSpace, 0);
    btnTipRect.moveTo(topLeft);
    painter.drawText(btnTipRect, Qt::AlignTop|Qt::AlignHCenter, "Exit\nGame");

    nSpace = int(24*m_devYFactor);
    topLeft = m_pUpBtn->frameGeometry().bottomLeft() + QPoint(0, nSpace);
    btnTipRect.setWidth(m_pUpBtn->width());
    btnTipRect.moveTo(topLeft);
    painter.drawText(btnTipRect, Qt::AlignTop|Qt::AlignHCenter, "Level");

    topLeft = m_pLeftBtn->frameGeometry().bottomLeft() + QPoint(0, int(nSpace*.6));
    btnTipRect.moveTo(topLeft);
    painter.drawText(btnTipRect, Qt::AlignTop|Qt::AlignHCenter, "Prev\nGame");

    topLeft = m_pRightBtn->frameGeometry().bottomLeft() + QPoint(0, int(nSpace*.6));
    btnTipRect.moveTo(topLeft);
    painter.drawText(btnTipRect, Qt::AlignTop|Qt::AlignHCenter, "Next\nGame");

    topLeft = m_pDownBtn->frameGeometry().bottomLeft() + QPoint(0, nSpace);
    btnTipRect.moveTo(topLeft);
    painter.drawText(btnTipRect, Qt::AlignTop|Qt::AlignHCenter, "Speed");

    topLeft = m_pRotateBtn->frameGeometry().bottomLeft() + QPoint(0, nSpace);
    btnTipRect.moveTo(topLeft);
    btnTipRect.setWidth(m_pRotateBtn->width());
    painter.drawText(btnTipRect, Qt::AlignTop|Qt::AlignHCenter, "Enter");
}

void CWidget::DrawGameBoard(QPainter &painter)
{
    if(m_pGameMgr == nullptr) return;

    GAMEBOARD pGmBd = m_pGameMgr->GetGameBoard();
    if(pGmBd == nullptr) return;

    int nRowsInGmBd = m_pGameMgr->GetGameBoardRowCount();
    int nColsInGmBd = m_pGameMgr->GetGameBoardColCount();

    // draw filled cells
    for(int i=0; i<nRowsInGmBd; ++i)
        for(int j=0; j<nColsInGmBd; ++j)
            if(pGmBd[i][j].IsFilled())
                paintCell(painter, pGmBd[i][j]);
}

void CWidget::DrawGameBoardBgr(QPainter &painter)
{
    if(m_pGameMgr == nullptr) return;

    GAMEBOARD pGmBd = m_pGameMgr->GetGameBoard();
    if(pGmBd == nullptr) return;

    int nRowsInGmBd = m_pGameMgr->GetGameBoardRowCount();
    int nColsInGmBd = m_pGameMgr->GetGameBoardColCount();

    for(int i=0; i<nRowsInGmBd; ++i)
        for(int j=0; j<nColsInGmBd; ++j)
            paintCell(painter, pGmBd[i][j]);
}

void CWidget::DrawMovingShape(QPainter &painter)
{
    GAMEBOARD pGmBd = m_pGameMgr->GetGameBoard();
    if(pGmBd == nullptr) return;

    CShape *pShape = m_pGameMgr->GetMovingShape();
    if(!pShape) return;

    CShapeState *pState = pShape->getCurrentState();
    if(!pState) return;

    CCell cell;
    for(int ii=0; ii<4; ++ii)
    {
        int xpos = pState->m_cellPoint[ii].x() + pShape->getShapePosition()->x();
        int ypos = pState->m_cellPoint[ii].y() + pShape->getShapePosition()->y();

        if(0<= xpos && xpos < m_pGameMgr->GetGameBoardRowCount() &&
                0<=ypos && ypos < m_pGameMgr->GetGameBoardColCount())
        {
            cell = pGmBd[xpos][ypos];
            cell.SetColor(pShape->getColor());
            paintCell(painter, cell, true);
        }
    }
}

void CWidget::DrawPreviewBoard(QPainter &painter)
{
    if(m_pGameMgr == nullptr) return;

    PREVIEWBOARD pPreviewBoard = m_pGameMgr->GetPreviewBoard();
    if(pPreviewBoard == nullptr) return;

    int nPreviewBdSz = m_pGameMgr->GetPreviewBoardSize();

    for(int i=0; i<nPreviewBdSz; ++i)
        for(int j=0; j<nPreviewBdSz; ++j)
            if(pPreviewBoard[i][j].IsFilled())
                paintCell(painter, pPreviewBoard[i][j]);
}

void CWidget::DrawPreviewBoardBgr(QPainter &painter)
{
    if(m_pGameMgr == nullptr) return;

    PREVIEWBOARD pPreviewBoard = m_pGameMgr->GetPreviewBoard();
    if(pPreviewBoard == nullptr) return;

    int nPreviewBdSz = m_pGameMgr->GetPreviewBoardSize();
    for(int i=0; i<nPreviewBdSz; ++i)
        for(int j=0; j<nPreviewBdSz; ++j)
            paintCell(painter, pPreviewBoard[i][j]);
}

void CWidget::DrawSideCells(QPainter &painter)
{
    vector<CCell>::iterator bgn = m_sideCells.begin();
    for(; bgn!=m_sideCells.end(); ++bgn)
    {
        CCell cell = *bgn;
        paintCell(painter, cell);
    }
}

void CWidget::DrawTitleAndTopFrameLine(QPainter &painter, QPen &pen)
{
    int SolidLen = int(150*m_devXFactor), DotLen = int(110*m_devXFactor);
    QPointF x0 = m_blackFrm.topLeft()+QPointF(SolidLen, 0);
    QPointF x1 = x0 + QPointF(DotLen, 0);
    QPointF x2 = m_blackFrm.topRight() + QPointF(-SolidLen, 0);
    QPointF x3 = x2 + QPointF(-DotLen, 0);

    // title position calculate
    QRectF titleRt(0, 0, (x3.x()-x1.x()), qreal(m_pStartBtn->size().width()));
    titleRt.moveTo(x1.x(), x1.y()-titleRt.height()/2);
    m_digitFont.setItalic(true);
    painter.setFont(m_digitFont);
    painter.drawText(titleRt, Qt::AlignCenter, "PERSEUS");
    m_digitFont.setItalic(false);

    // draw the top line
    painter.drawLine(m_blackFrm.topLeft(), x0);
    painter.drawLine(m_blackFrm.topRight(), x2);
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    painter.drawLine(x0, x1);
    painter.drawLine(x2, x3);
}

void CWidget::DrawBump(QPainter &painter)
{
    // top
    qreal xmargin = m_tbumpFrm.height();
    QPointF pt0 = QPointF(0,0),
            pt1 = QPointF(0,m_height),
            pt2 = QPointF(m_width, m_height),
            pt3 = QPointF(m_width, 0),
            pt4 = QPointF(xmargin, m_tbumpFrm.bottom()),
            pt5 = QPointF(xmargin, m_bbumpFrm.top()),
            pt6 = QPointF(m_width-xmargin, m_bbumpFrm.top()),
            pt7 = QPointF(m_width-xmargin, m_tbumpFrm.bottom());

    QLinearGradient grd;
    grd.setStart(m_tbumpFrm.bottomLeft());
    grd.setFinalStop(m_tbumpFrm.topLeft());
    grd.setColorAt(1, QColor("#665E0B"));
    grd.setColorAt(0, bgr);
    QVector<QPointF> pts{pt0, pt4, pt7, pt3, pt0};
    QPainterPath path0;
    path0.addPolygon(pts);
    painter.fillPath(path0, QBrush(grd));

    // left
    grd.setStart(pt4);
    grd.setFinalStop(QPointF(0, pt4.y()));
    pts.clear();
    pts << pt0 << pt1 << pt5 << pt4 << pt0;
    QPainterPath path1;
    path1.addPolygon(pts);
    painter.fillPath(path1, QBrush(grd));

    // bottom
    grd.setStart(pt5);
    grd.setFinalStop(QPointF(pt5.x(), m_height));
    pts.clear();
    pts << pt5 << pt1 << pt2 << pt6 << pt5;
    QPainterPath path2;
    path2.addPolygon(pts);
    painter.fillPath(path2, QBrush(grd));

    // right
    grd.setStart(pt6);
    grd.setFinalStop(QPointF(m_width, pt6.y()));
    pts.clear();
    pts << pt6 << pt2 << pt3 << pt7 << pt6;
    QPainterPath path3;
    path3.addPolygon(pts);
    painter.fillPath(path3, QBrush(grd));

    QLinearGradient grd0(m_bumpFrm.topLeft(), m_bumpFrm.bottomLeft());
#define USE_FACTOR
#ifdef USE_FACTOR
    grd0.setColorAt(0, bgr);
    grd0.setColorAt(.02, bgr);
    grd0.setColorAt(.1, QColor("#DCCC76"));
    grd0.setColorAt(.12, QColor("#DCCC76"));
    grd0.setColorAt(.2, QColor("#DCCC76"));
    grd0.setColorAt(.3, QColor("#BDA11B"));
    grd0.setColorAt(.4, QColor("#BDA111"));
    grd0.setColorAt(.41, QColor("#BCA011"));
    grd0.setColorAt(.45, QColor("#B99E10"));
    grd0.setColorAt(.6, QColor("#B99E10"));
    grd0.setColorAt(.8, QColor("#867703"));
    grd0.setColorAt(.9, QColor("#867703"));
    grd0.setColorAt(1, bgr);
#else
    qreal height = 346-290;
    grd0.setColorAt((297-290)/height, QColor("#DBBD15"));
    grd0.setColorAt((298-290)/height, QColor(/*"#DCC963"*/"#DCCC76"));
    grd0.setColorAt((299-290)/height, QColor(/*"#DCC963"*/"#DCCC76"));
    grd0.setColorAt((304-290)/height, QColor(/*"#DCC963"*/"#DCCC76"));
    grd0.setColorAt((305-290)/height, QColor("#DBD090"));
    grd0.setColorAt((309-290)/height, QColor("#D2C378"));
    grd0.setColorAt((311-290)/height, QColor("#C4AE37"));
    grd0.setColorAt((320-290)/height, QColor("#BCA111"));
    grd0.setColorAt((330-290)/height, QColor("#A58E10"));
    grd0.setColorAt((340-290)/height, QColor("#95810E"));
    grd0.setColorAt((345-220)/height, bgr);
#endif

//#define USE_PICTURE
#ifdef USE_PICTURE
    QLabel *pBump = new QLabel(this);
    pBump->resize(m_bumpFrm.width(), m_bumpFrm.height());
    pBump->move(QPoint(m_bumpFrm.left(), m_bumpFrm.top()));
    pBump->setStyleSheet("QLabel{border-image:url(:/img/img/bump.png)}");
#else
    QPainterPath rrPath;
    qreal xRadius = m_bumpFrm.height()*.3;
    qreal yRadius = xRadius;
    rrPath.addRoundedRect(m_bumpFrm, xRadius, yRadius);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillPath(rrPath, QBrush(grd0));
#endif
}

void CWidget::DrawBoyHandsUp(QPainter &painter)
{
    /*
#ifdef ANDROID
    qreal pensz = .1;
#else
    qreal pensz = .5;
#endif

    static size_t ptCnt = NELEMENTS(handup136);

    QPen pen;
    pen.setWidthF(pensz);
    painter.setPen(pen);    
    painter.drawPoints(handup136, int(ptCnt));*/

    QPixmap handdown(":/img/img/handup.png");
    qreal pxW = handdown.width();
    qreal pxH = handdown.height();
    qreal factor = m_flashFrm.height() / pxH;
    handdown = handdown.scaled(int(pxW*factor),int(pxH*factor),Qt::KeepAspectRatio);
    QPointF pt = m_flashFrm.topLeft() + QPointF((m_flashFrm.width()-pxW*factor)/2, 0);
    painter.drawPixmap(pt, handdown);
}

void CWidget::DrawBoyHandsDown(QPainter &painter)
{
    /*
#ifdef ANDROID
    qreal pensz = 1;
#else
    qreal pensz = .5;
#endif

    static size_t ptCnt = NELEMENTS(handdown100);

    QPen pen;
    pen.setWidthF(pensz);
    painter.setPen(pen);
    painter.drawPoints(handdown100, int(ptCnt));*/

    QPixmap handdown(":/img/img/handdown.png");
    qreal pxW = handdown.width();
    qreal pxH = handdown.height();
    qreal factor = m_flashFrm.height() / pxH;
    handdown = handdown.scaled(int(pxW*factor),int(pxH*factor),Qt::KeepAspectRatio);
    QPointF pt = m_flashFrm.topLeft() + QPointF((m_flashFrm.width()-pxW*factor)/2, 0);
    painter.drawPixmap(pt, handdown);
}

void CWidget::DrawBackground()
{
    if(m_pxbgr == nullptr)
        m_pxbgr = new QPixmap(m_width, m_height);

    QPainter painter(m_pxbgr);
    painter.setRenderHint(QPainter::Antialiasing);

    // change background color
    painter.fillRect(rect(), bgr);

    // draw black frame and title
    QPen pen;
    pen.setWidthF(15*m_devXFactor);
    painter.setPen(pen);
    painter.drawLine(m_blackFrm.topLeft(), m_blackFrm.bottomLeft());
    painter.drawLine(m_blackFrm.bottomLeft(), m_blackFrm.bottomRight());
    painter.drawLine(m_blackFrm.bottomRight(), m_blackFrm.topRight());
    DrawTitleAndTopFrameLine(painter, pen);

    // draw the sink frame
    DrawSinkFrame(painter);

    // draw boy
    //DrawBoyHandsUp(painter);
    DrawBoyHandsDown(painter);

    // draw game board frame
    pen.setStyle(Qt::SolidLine);
    pen.setWidthF(1.0*m_devXFactor);
    painter.setPen(pen);
    painter.drawRect(m_gmbdFrame.adjusted(0,0,-1,0));
    painter.fillRect(m_gmbdFrame.adjusted(0,0,-1,0), sinkBgr);

    DrawBump(painter);

    DrawSideCells(painter);

    DrawStaticText(painter);

    DrawGameBoardBgr(painter);

    DrawPreviewBoardBgr(painter);

    DrawBtnCircles(painter);

    painter.end();
}

void CWidget::DrawForeground()
{
    if(m_pxfgr == nullptr)
        m_pxfgr = new QPixmap(m_width, m_height/2);

    QPainter painter(m_pxfgr);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setRenderHint(QPainter::RenderHint(0x1F));
    painter.eraseRect(m_sinkFrame);
    painter.fillRect(m_pxfgr->rect(), Qt::transparent);

    DrawGameText(painter);
    DrawGameBoard(painter);
    DrawMovingShape(painter);
    DrawPreviewBoard(painter);
    painter.end();
}

void CWidget::DrawSoundSymb()
{
    QPainter painter(m_pxbgr);
    painter.setRenderHint(QPainter::Antialiasing);

    static QPixmap sound(":/img/img/sound.png");
    static QPixmap mute(":/img/img/mute.png");
    QPixmap *symb = (m_bVoiceOn ? &sound : &mute);
    qreal pxW = symb->width();
    qreal pxH = symb->height();
    qreal factor = m_soundFrm.height() / pxH;
    *symb = symb->scaled(int(pxW*factor),int(pxH*factor),Qt::KeepAspectRatio);
    QPointF pt = m_soundFrm.topLeft() + QPointF((m_soundFrm.width()-pxW*factor)/2, 0);
    painter.drawPixmap(pt, *symb);
}

void CWidget::DrawPausedSymb()
{
    QPainter painter(m_pxbgr);
    painter.setRenderHint(QPainter::Antialiasing);

    static QPixmap goon(":/img/img/goon.png");
    static QPixmap paused(":/img/img/paused.png");
    QPixmap *symb = (m_bPaused ? &paused : &goon);
    qreal pxW = symb->width();
    qreal pxH = symb->height();
    qreal factor = m_pauseFrm.height() / pxH;
    *symb = symb->scaled(int(pxW*factor), int(pxH*factor), Qt::KeepAspectRatio);
    QPointF pt = m_pauseFrm.topLeft() + QPointF((m_pauseFrm.width()-pxW*factor)/2*.2, 0);
    painter.drawPixmap(pt, *symb);
}

void CWidget::mousePressEvent(QMouseEvent *event)
{
#ifdef ANDROID
    Q_UNUSED(event)
#else
    m_wndPos = event->globalPos();
#endif
}

void CWidget::mouseMoveEvent(QMouseEvent *event)
{
#ifdef ANDROID
    Q_UNUSED(event)
#else
    QPoint offset = event->globalPos() - m_wndPos;
    m_wndPos = event->globalPos();
    move(offset + QPoint(x(), y()));
#endif
}

void CWidget::mouseReleaseEvent(QMouseEvent *event)
{
#ifdef ANDROID
    Q_UNUSED(event)
#else
    QPoint offset = event->globalPos() - m_wndPos;
    move(offset + QPoint(x(), y()));
#endif
}

void CWidget::slotButtonClicked()
{
#ifndef USE_IMGBTN
    slotOnVibrate(30);
#endif

    QString name = sender()->objectName();
    qDebug() << "push button " << name << " triggered." << endl;

    if(0 == name.compare(QString("upButton"), Qt::CaseInsensitive))
    {
        emit sigPushButton(CGameManager::MD_Up);
    }
    else if(0 == name.compare(QString("downButton"), Qt::CaseInsensitive))
    {
        emit sigPushButton(CGameManager::MD_Down);
    }
    else if(0 == name.compare(QString("leftButton"), Qt::CaseInsensitive))
    {
        emit sigPushButton(CGameManager::MD_Left);
    }
    else if(0 == name.compare(QString("rightButton"), Qt::CaseInsensitive))
    {
        emit sigPushButton(CGameManager::MD_Right);
    }
    else if(0 == name.compare(QString("rotateButton"), Qt::CaseInsensitive))
    {
        emit sigPushButton(CGameManager::MD_Rotate);
    }
    else if(0 == name.compare(QString("startButton"), Qt::CaseInsensitive))
    {
        emit sigAction(CGameManager::GA_NewGame);
        repaint(m_updateRt);
    }
    else if(0 == name.compare(QString("soundButton"), Qt::CaseInsensitive))
    {
        static int volume = m_player->volume();
        m_bVoiceOn = !m_bVoiceOn;
        m_player->setVolume((m_bVoiceOn? volume : 0));

        DrawSoundSymb();
        repaint(m_updateRt);
    }
    else if(0 == name.compare(QString("settingButton"), Qt::CaseInsensitive))
    {

    }
    else if(0 == name.compare(QString("exitButton"), Qt::CaseInsensitive))
    {
        exit(0);
    }
    else
    {

    }
}

void CWidget::slotRefresh()
{
    repaint(m_updateRt);
}

void CWidget::slotOnVibrate(int ms)
{
#ifdef ANDROID
    if(m_bVibrateOn)
    {
        qDebug() << "slotOnVibrate()" << endl;
        QAndroidJniEnvironment env;
        QAndroidJniObject activity = androidActivity();
        QAndroidJniObject name = QAndroidJniObject::getStaticObjectField(
                    "android/content/Context",
                    "VIBRATOR_SERVICE",
                    "Ljava/lang/String;"
                    );
        CHECK_EXCEPTION()
        QAndroidJniObject vibrateService = activity.callObjectMethod(
                    "getSystemService",
                    "(Ljava/lang/String;)Ljava/lang/Object;",
                    name.object<jstring>());
        CHECK_EXCEPTION()
        jlong duration = ms;
        vibrateService.callMethod<void>("vibrate", "(J)V", duration);
        CHECK_EXCEPTION()
    }
#else
    Q_UNUSED(ms)
#endif
}

void CWidget::slotPlayAudio(int nIndex)
{
    switch (nIndex)
    {
    case 0: // bgm
    {
        m_player->setMedia(QUrl("qrc:/localAudio/audio/Tetris/StartGame.mp3"));
        m_player->play();

        break;
    }
    case 1: // clear line
    {
        m_player->setMedia(QUrl("qrc:/localAudio/audio/Tetris/clearLine.mp3"));
        m_player->play();
        break;
    }
    case 2: // hit the brick
    {
        m_player->setMedia(QUrl("qrc:/localAudio/audio/Tetris/hit.mp3"));
        m_player->play();
        break;
    }
    default:
        break;
    }
}

void CWidget::InitConnectors()
{
    // UI to game manager
    connect(this, SIGNAL(sigInitGameMgr(QPoint, QPoint, int, int)), m_pGameMgr, SLOT(slotInitGameManager(QPoint, QPoint, int, int)));
    connect(this, SIGNAL(sigAction(int)), m_pGameMgr, SLOT(slotGameAction(int)));
    connect(this, SIGNAL(sigPushButton(int)), m_pGameMgr, SLOT(slotMoveDirection(int)));

    // game manager to UI
    connect(m_pGameMgr, SIGNAL(sigUpdateUI()), this, SLOT(slotRefresh()));
    connect(m_pGameMgr, SIGNAL(sigStartGame(int)), this, SLOT(slotPlayAudio(int)));
}

void CWidget::InitSettingBtns()
{
#ifdef USE_IMGBTN
    m_pStartBtn = new CJBtn(this);
    m_pSoundBtn = new CJBtn(this);
    m_pSettingBtn = new CJBtn(this);
    m_pExitBtn = new CJBtn(this);
#else
    m_pStartBtn = new QPushButton(this);
    m_pSoundBtn = new QPushButton(this);
    m_pSettingBtn = new QPushButton(this);
    m_pExitBtn = new QPushButton(this);
#endif

    InitSettingBtn(m_pStartBtn, tr("startButton"));
    InitSettingBtn(m_pSoundBtn, tr("soundButton"));
    InitSettingBtn(m_pSettingBtn, tr("settingButton"));
    InitSettingBtn(m_pExitBtn, tr("exitButton"));

    int btnTop = int(1192 * m_devYFactor + m_tMargin);
    m_pStartBtn->move(int(565*m_devXFactor+m_lMargin), btnTop);
    m_pSoundBtn->move(int(696*m_devXFactor+m_lMargin), btnTop);
    m_pSettingBtn->move(int(827*m_devXFactor+m_lMargin), btnTop);
    m_pExitBtn->move(int(958*m_devXFactor+m_lMargin), btnTop);
}

void CWidget::InitSettingBtn(QPushButton *pBtn, const QString &objName)
{
    pBtn->setObjectName(objName);
    QSize btnSize(int(68*m_devXFactor), int(68*m_devXFactor));
    pBtn->setFixedSize(btnSize);
#ifdef USE_IMGBTN
    CJBtn *pJBtn = static_cast<CJBtn*>(pBtn);
    pJBtn->setStyleImg(QString(":/img/img/gnormal.png"), QString(":/img/img/gpressed.png"));
    pJBtn->setRepeat(false);
    connect(pJBtn, SIGNAL(sigVibrate(int)), this, SLOT(slotOnVibrate(int)));
#else
    pBtn->setStyleSheet("QPushButton{border-image: url(:/img/img/gnormal.png);}"
                        "QPushButton:hover{border-image: url(:/img/img/gnormal.png);}"
                        "QPushButton:pressed{border-image: url(:/img/img/gpressed.png);}");
#endif

    // connect signal and slot
    connect(pBtn, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
}

void CWidget::InitGameBtns()
{
    InitSettingBtns();
    InitDirectionBtns();
    InitRotateBtn();
}

void CWidget::InitDirectionBtns()
{
#ifdef USE_IMGBTN
    m_pUpBtn = new CJBtn(this);
    m_pDownBtn = new CJBtn(this);
    m_pLeftBtn = new CJBtn(this);
    m_pRightBtn = new CJBtn(this);
#else
    m_pUpBtn = new QPushButton(this);
    m_pDownBtn = new QPushButton(this);
    m_pLeftBtn = new QPushButton(this);
    m_pRightBtn = new QPushButton(this);
#endif

    InitDirectionBtn(m_pUpBtn, tr("upButton"));
    InitDirectionBtn(m_pDownBtn, tr("downButton"));
    InitDirectionBtn(m_pLeftBtn, tr("leftButton"));
    InitDirectionBtn(m_pRightBtn, tr("rightButton"));

    m_pUpBtn->move(int(218*m_devXFactor+m_lMargin), int(1277*m_devYFactor+m_tMargin));
    m_pLeftBtn->move(int(48*m_devXFactor+m_lMargin), int(1439*m_devYFactor+m_tMargin));
    m_pRightBtn->move(int(377*m_devXFactor+m_lMargin), int(1439*m_devYFactor+m_tMargin));
    m_pDownBtn->move(int(218*m_devXFactor+m_lMargin), int(1595*m_devYFactor+m_tMargin));
}

void CWidget::InitDirectionBtn(QPushButton *pBtn, const QString &objName)
{
    pBtn->setObjectName(objName);
    pBtn->setFocusPolicy(Qt::NoFocus);
    static QSize btnSize(int(188*m_devXFactor), int(188*m_devXFactor));
    pBtn->setFixedSize(btnSize);

#ifdef USE_IMGBTN
    CJBtn *pJBtn = static_cast<CJBtn*>(pBtn);
    pJBtn->setStyleImg(QString(":/img/img/ynormal.png"), QString(":/img/img/ypressed.png"));
    connect(pJBtn, SIGNAL(sigVibrate(int)), this, SLOT(slotOnVibrate(int)));
#else
    pBtn->setAutoRepeat(true);
    pBtn->setAutoRepeatDelay(200);
    pBtn->setAutoRepeatInterval(50);
    pBtn->setStyleSheet("QPushButton{border-image: url(:/img/img/ynormal.png);}"
                        "QPushButton:hover{border-image: url(:/img/img/ynormal.png);}"
                        "QPushButton:pressed{border-image: url(:/img/img/ypressed.png);}");
#endif

    // connect signal and slot
    connect(pBtn, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));    
}

void CWidget::InitRotateBtn()
{
#ifdef USE_IMGBTN
    m_pRotateBtn = new CJBtn(this);
    CJBtn* pJBtn = static_cast<CJBtn*>(m_pRotateBtn);
    pJBtn->setStyleImg(QString(":/img/img/ynormal.png"), QString(":/img/img/ypressed.png"));
    connect(pJBtn, SIGNAL(sigVibrate(int)), this, SLOT(slotOnVibrate(int)));
#else
    m_pRotateBtn = new QPushButton(this);
    m_pRotateBtn->setAutoRepeat(true);
    m_pRotateBtn->setAutoRepeatDelay(200);
    m_pRotateBtn->setAutoRepeatInterval(50);
    m_pRotateBtn->setStyleSheet("QPushButton{border-image: url(:/img/img/ynormal.png);}"
                                "QPushButton:hover{border-image: url(:/img/img/ynormal.png);}"
                                "QPushButton:pressed{border-image: url(:/img/img/ypressed.png);}");
#endif

    QSize btnSize(int(281*m_devXFactor), int(281*m_devXFactor));
    m_pRotateBtn->setFixedSize(btnSize);
    m_pRotateBtn->setObjectName(tr("rotateButton"));
    m_pRotateBtn->setFocusPolicy(Qt::NoFocus);
    m_pRotateBtn->move(int(698*m_devXFactor+m_lMargin), int(1412*m_devYFactor+m_tMargin));

    // connect signal and slot
    connect(m_pRotateBtn, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
}

void CWidget::InitPositon()
{
    InitGameBoardFrame();
    InitPreviewBoardFrame();
    InitScoreTxtFrame();
    InitScoreValFrame();
    InitHiScoreTxtFrame();
    InitHiScoreValFrame();
    InitLevelFrame();
    InitSpeedFrame();
    InitFlashFrame();
    InitMarkFrame();
    InitBumpFrame();
    InitGameMgrPos();
    InitSideShapes();
}

void CWidget::InitGameBoardFrame()
{
    QPointF tl(m_lMargin+164*m_devXFactor, 100*m_devYFactor);
    QPointF br(m_lMargin+906*m_devXFactor, 991*m_devYFactor);
    m_blackFrm = QRectF(tl, br);

    tl = QPointF(m_lMargin+0*m_devXFactor, 100*m_devYFactor);
    br = QPointF(m_lMargin+164*m_devXFactor, 991*m_devYFactor);
    m_leftSideFrm = QRectF(tl, br);

    tl = QPointF(m_lMargin+906*m_devXFactor, 100*m_devYFactor);
    br = QPointF(m_lMargin+1080*m_devXFactor, 991*m_devYFactor);
    m_rightSideFrm = QRectF(tl, br);

    tl = QPointF(m_lMargin+188*m_devXFactor, 129*m_devYFactor);
    br = QPointF(m_lMargin+885*m_devXFactor, 973*m_devYFactor);
    m_sinkFrame = QRectF(tl, br);

    tl = QPointF(m_lMargin+220*m_devXFactor, 158*m_devYFactor);
    br = QPointF(m_lMargin+622*m_devXFactor, 942*m_devYFactor);
    m_gmbdFrame = QRectF(tl, br);

    qreal difference = m_gmbdFrame.width()*2-m_gmbdFrame.height();
    if(difference > 0)
    {
        m_gmbdFrame.setHeight(m_gmbdFrame.width()*2);
        m_blackFrm.setHeight(m_blackFrm.height()+difference);
        m_leftSideFrm.setHeight(m_leftSideFrm.height()+difference);
        m_rightSideFrm.setHeight(m_rightSideFrm.height()+difference);
        m_sinkFrame.setHeight(m_sinkFrame.height()+difference);
    }

    if(m_scrType == Longer)
        qDebug() << "Longer Game Board Rect" << endl;
    else
        qDebug() << "Wider Game Board Rect, margin = " << m_lMargin << endl;
}

void CWidget::InitPreviewBoardFrame()
{
    qreal prbdWidth = (m_gmbdFrame.width()-2.0) * 0.4;
    QSizeF prbdSize(prbdWidth, prbdWidth);

    qreal dLeftMargin = (m_sinkFrame.right()-m_gmbdFrame.right()-prbdWidth)/2;
    qreal left = m_gmbdFrame.right() + dLeftMargin;

    qreal dTopMargin = (m_gmbdFrame.height() - prbdWidth)/2;
    qreal top = m_gmbdFrame.top() + dTopMargin - prbdWidth/4;

    m_prbdFrame = QRectF(QPointF(left, top), prbdSize);
}

void CWidget::InitScoreTxtFrame()
{
    qreal dscrW = m_sinkFrame.right() - m_gmbdFrame.right();
    qreal dscrH = m_gmbdFrame.width()/m_pGameMgr->GetGameBoardColCount();
    QPointF tl = m_gmbdFrame.topRight();
    m_scoreStrFrm = QRectF(tl, QSizeF(dscrW, dscrH));
}

void CWidget::InitScoreValFrame()
{
    QPointF tl = m_scoreStrFrm.bottomLeft() + QPointF(0, m_scoreStrFrm.height() / 3);
    m_scoreNumFrm = QRectF(tl, m_scoreStrFrm.size());
}

void CWidget::InitHiScoreTxtFrame()
{
    QPointF tl = m_scoreNumFrm.bottomLeft() + QPointF(0, m_scoreNumFrm.height()/3);
    m_hiscrStrFrm = QRectF(tl, m_scoreStrFrm.size());
}

void CWidget::InitHiScoreValFrame()
{
    QPointF tl = m_hiscrStrFrm.bottomLeft() + QPointF(0, m_hiscrStrFrm.height()/3);
    m_hiscrNumFrm = QRectF(tl, m_hiscrStrFrm.size());
}

void CWidget::InitLevelFrame()
{
    qreal l = m_gmbdFrame.right();
    qreal t = m_prbdFrame.bottom() + m_hiscrNumFrm.height();
    m_levelFrm = QRectF(QPointF(l, t), m_hiscrStrFrm.size());
}

void CWidget::InitSpeedFrame()
{
    QPointF tl = m_levelFrm.bottomLeft() + QPointF(0, m_levelFrm.height()/3);
    m_speedFrm = QRectF(tl, m_levelFrm.size());
}

void CWidget::InitFlashFrame()
{    
    qreal margin = 2;
    QPointF tl = m_speedFrm.bottomLeft() + QPointF(2*margin, m_speedFrm.height()/3);
    qreal width = m_speedFrm.width() - 2 * margin - 6 * m_devXFactor;
    qreal height = m_sinkFrame.bottom() - tl.y() - 2 * m_speedFrm.height();
    m_flashFrm = QRectF(tl, QSizeF(width, height));

    InitFlashData();
}

void CWidget::InitFlashData()
{
    // init flash boy data position and resize flash frame
#ifdef ANDROID
    qreal boySzFactor = 1.0;
#else
    qreal boySzFactor = .4;
#endif
    QPointF pt(m_flashFrm.topLeft());
    qreal xmin = m_flashFrm.right(),  xmax = m_flashFrm.left();
    qreal ymin = m_flashFrm.bottom(), ymax = m_flashFrm.top();
    size_t ptCnt0 = NELEMENTS(handup136), ptCnt1 = NELEMENTS(handdown100);

    for(size_t i=0; i<ptCnt0; ++i)
    {
        QPointF *pt0 = &handup136[i];
        *pt0 = pt + QPointF(pt0->y(), pt0->x())*boySzFactor;
        xmin = qMin(xmin, pt0->x());
        xmax = qMax(xmax, pt0->x());
        ymin = qMin(ymin, pt0->y());
        ymax = qMax(ymax, pt0->y());
    }

    for(size_t i=0; i<ptCnt1; ++i)
    {
        QPointF *pt0 = &handdown100[i];
        *pt0 = pt + QPointF(pt0->y(), pt0->x())*boySzFactor;
        xmin = qMin(xmin, pt0->x());
        xmax = qMax(xmax, pt0->x());
        ymin = qMin(ymin, pt0->y());
        ymax = qMax(ymax, pt0->y());
    }

    qreal w = xmax - xmin;
    qreal h = ymax - ymin;
    qreal x = (m_sinkFrame.right()-m_gmbdFrame.right()-6*m_devXFactor-w)/2;
    for(size_t i=0; i<ptCnt0; ++i)
        handup136[i] += QPointF(x, 0);
    for(size_t i=0; i<ptCnt1; ++i)
        handdown100[i] += QPointF(x, 0);

    m_flashFrm.setHeight(h+2);
}

void CWidget::InitMarkFrame()
{
    qreal tmargin = 1*m_devYFactor;
    qreal lmargin = 1*m_devXFactor;
    QPointF tl = m_flashFrm.bottomLeft() + QPointF(lmargin, tmargin);
    QSizeF sz(m_speedFrm.width()/2-lmargin, m_speedFrm.height()*2);
    m_soundFrm = QRectF(tl, QSizeF(sz.width(), sz.height()*.85));

    tl += QPointF(m_speedFrm.width()/2, 0);
    m_pauseFrm = QRectF(tl, sz);

    m_soundFrm.moveTo(m_soundFrm.left(), m_soundFrm.top()+sz.height()*.15);
}

void CWidget::InitBumpFrame()
{
    // side bump rect
    qreal h0 = 20 * m_devYFactor;
    m_tbumpFrm = QRectF(QPointF(0, 0), QPointF(m_width, h0));
    m_bbumpFrm = QRectF(QPointF(0, m_height-h0), QPointF(m_width, m_height));
    m_lbumpFrm = QRectF(QPointF(0, 0), QPointF(m_leftSideFrm.left(), m_height));
    m_rbumpFrm = QRectF(QPointF(m_rightSideFrm.right(), 0), QPointF(m_width, m_height));

    qreal y0 = qMax(1080*m_devYFactor, m_blackFrm.bottom()+1*m_devXFactor);
    qreal y1 = m_pStartBtn->frameGeometry().top() + m_pStartBtn->size().width()/4;
    m_bumpFrm = QRectF(QPointF(h0, y0), QPointF(m_width-h0, y1));
    //m_bumpFrm = QRectF(QPointF(0, 1009*m_devYFactor), QPointF(1080*m_devYFactor, 1210*m_devYFactor));
    y1 = qMax(m_blackFrm.bottom()+20*m_devYFactor, 1009*m_devYFactor)+m_tMargin/2;
    qreal y2 = qMin(1210*m_devYFactor, m_pStartBtn->frameGeometry().top()-2*m_devYFactor)+m_tMargin/2;
    QPointF tl(h0, y1), br(m_width-h0, y2);
    m_bumpFrm = QRectF(tl, br);
}

void CWidget::InitGameMgrPos()
{
    QPoint tl(int(m_gmbdFrame.left()), int(m_gmbdFrame.top()));
    QPoint ptl(int(m_prbdFrame.left()), int(m_prbdFrame.top()));
    int width = int(m_gmbdFrame.width());
    int height= int(m_gmbdFrame.height());
    emit sigInitGameMgr(tl, ptl, width, height);
}

void CWidget::InitSideShapes()
{
    GAMEBOARD pGmBd = m_pGameMgr->GetGameBoard();
    if(pGmBd == nullptr) return;

    CCell cell = pGmBd[0][0];
    cell.FillCell();
    cell.SetColor(Qt::black);

    qreal x0 = (m_leftSideFrm.width() - 2*cell.width())/2 + cell.width() + m_lMargin;
    qreal y0 = (m_leftSideFrm.height() - 23 * cell.height())/2 + m_leftSideFrm.top();
    qreal x1 = (m_rightSideFrm.width()- 2*cell.width())/2 + m_rightSideFrm.left() + m_lMargin;
    qreal y1 = y0;

#define o QPoint(0, 0)
#define r QPoint(1, 0)
#define d QPoint(0, 1)
#define d2 QPoint(0, 2)
#define ld QPoint(-1, 1)
#define ld2 QPoint(-1, 2)
#define rd QPint(1, 1)
#define rd2 QPoint(1, 2)

    QPoint posTbl[] =
    {
        o, ld, r, ld,   // z
        d2, d, r, ld,   // t
        d2, r, ld, r,   // square
        ld2, d, r, d,   // s
        ld2, r, d, d,   // L
        ld2, d, d, d    // line
    };
    QPoint posTbl1[] =
    {
        o, d, r, d,     // s
        d2, ld, r, d,   // t
        ld2, r, ld, r,  // square
        d2, ld, r, ld,  // z,
        d2, r, ld, d,   // j
        rd2, d, d, d,   // line
    };

    cell.moveTo(int(x0), int(y0));
    for (int i=0; i<24; ++i)
    {
        cell = cell.moved(posTbl[i]);
        m_sideCells.push_back(cell);
    }

    cell.moveTo(int(x1), int(y1));
    for (int i=0; i<24; ++i)
    {
        cell = cell.moved(posTbl1[i]);
        m_sideCells.push_back(cell);
    }
}







