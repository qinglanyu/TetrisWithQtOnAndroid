#ifndef CWIDGET_H
#define CWIDGET_H

#include <QWidget>
#include <vector>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class CWidget; }
QT_END_NAMESPACE

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class CGameManager;
class CCell;
class QMediaPlayer;
class QPointF;

const static qreal gIdealX = 1080;
const static qreal gIdealY = 1920;
const static qreal gIdealLWRatio = gIdealY / gIdealX;
enum devScrType {Wider, Longer};

class CWidget : public QWidget
{
    Q_OBJECT

public:
    CWidget(int iWidth, int iHeight, devScrType scrtype, qreal lmargin = 0, qreal tmargin = 0, QWidget *parent = nullptr);
    ~CWidget() override;

public:
    void InitUI();

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintCell(QPainter &painter, CCell &cell, bool moving=false);
    void DrawSinkFrame(QPainter &painter);
    void DrawBtnCircles(QPainter &painter);
    void DrawGameText(QPainter &painter);
    void DrawStaticText(QPainter &painter);
    void DrawGameBoard(QPainter &painter);
    void DrawGameBoardBgr(QPainter &painter);
    void DrawMovingShape(QPainter &painter);
    void DrawPreviewBoard(QPainter &painter);
    void DrawPreviewBoardBgr(QPainter &painter);
    void DrawSideCells(QPainter &painter);
    void DrawTitleAndTopFrameLine(QPainter &painter, QPen &pen);
    void DrawBump(QPainter &painter);
    void DrawBoyHandsUp(QPainter &painter);
    void DrawBoyHandsDown(QPainter &painter);
    void DrawBackground();
    void DrawForeground();
    void DrawSoundSymb();
    void DrawPausedSymb();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    void slotButtonClicked();
    void slotRefresh();
    void slotOnVibrate(int ms);
    void slotPlayAudio(int nIndex);

Q_SIGNALS:
    void sigInitGameMgr(QPoint gbTopLeft, QPoint pbTopLeft, int gbWidth, int gbHeight);
    void sigAction(int gameAction);
    void sigPushButton(int moveDirection);

protected:
    void InitConnectors();
    void InitSettingBtns();
    void InitSettingBtn(QPushButton* pBtn, const QString &objName);
    void InitGameBtns();
    void InitDirectionBtns();
    void InitDirectionBtn(QPushButton* pBtn, const QString &objName);
    void InitRotateBtn();
    void InitPositon();

    void InitGameBoardFrame();
    void InitPreviewBoardFrame();
    void InitScoreTxtFrame();
    void InitScoreValFrame();
    void InitHiScoreTxtFrame();
    void InitHiScoreValFrame();
    void InitLevelFrame();
    void InitSpeedFrame();
    void InitFlashFrame();
    void InitFlashData();
    void InitMarkFrame();
    void InitBumpFrame();
    void InitGameMgrPos();

    void InitSideShapes();

private:
    int m_width;
    int m_height;
    devScrType m_scrType;
    qreal m_lMargin;
    qreal m_tMargin;
    qreal m_devXFactor;
    qreal m_devYFactor;
    bool m_bVoiceOn;
    bool m_bPaused;
    bool m_bVibrateOn;

    QMediaPlayer  *m_player;

    QPushButton *m_pUpBtn;
    QPushButton *m_pDownBtn;
    QPushButton *m_pLeftBtn;
    QPushButton *m_pRightBtn;
    QPushButton *m_pRotateBtn;
    QPushButton *m_pStartBtn;
    QPushButton *m_pSoundBtn;
    QPushButton *m_pSettingBtn;
    QPushButton *m_pExitBtn;

    CGameManager *m_pGameMgr;

    QRectF m_blackFrm;
    QRectF m_sinkFrame;
    QRectF m_gmbdFrame;
    QRectF m_prbdFrame;
    QRect  m_updateRt;

    QRectF m_scoreStrFrm;
    QRectF m_scoreNumFrm;
    QRectF m_hiscrStrFrm;
    QRectF m_hiscrNumFrm;
    QRectF m_levelFrm;
    QRectF m_speedFrm;
    QRectF m_flashFrm;
    QRectF m_soundFrm;
    QRectF m_pauseFrm;

    QRectF m_bumpFrm;
    QRectF m_tbumpFrm;
    QRectF m_bbumpFrm;
    QRectF m_lbumpFrm;
    QRectF m_rbumpFrm;

    QRectF m_leftSideFrm;
    QRectF m_rightSideFrm;
    vector<CCell> m_sideCells;

    QFont  m_digitFont;

    QPixmap *m_pxbgr;       // background pixmap
    QPixmap *m_pxfgr;       // foreground pixmap
    QPixmap *m_pxscr;       // screen pixmap

    QPoint m_wndPos;
};
#endif // CWIDGET_H
