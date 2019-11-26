#ifndef CTETRIS_H
#define CTETRIS_H

#include "./CGame.h"
#include "gameglobal.h"
#include <QPoint>
#include <map>
using namespace std;

class CShape;
class CTetrisData;

class CTetris : public CGame
{
public:
    CTetris();
    ~CTetris();

public:
    bool InitGame();
    bool RunGame();
    bool PauseGame();
    bool ResetGame();   // reset game before start
    bool ReleaseGame(); // continue game after paused
    bool EndGame();

    bool InitTheGameData(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard);
    CShape *GetMovingShape();

    bool UpKeyDown();
    bool DownKeyDown();
    bool LeftKeyDown();
    bool RightKeyDown();
    bool RotateKeyDown();
    bool FloorKeyDown();    // space key
    bool HeartMsgHandle();  // falling down a shape

protected:
    void FinalEstimate();

public:
    bool FallingOneRow();
    bool FallingDownToBottom();
    CShape *GetFallingShape();
    int ClearLines();

    bool IsGameOver();
    void UpdateGrades(const int nCleardLines);
    void ResetGrades();

protected:
    CTetrisData *m_pTetrisData;
};


class CTetrisData : public CGameData
{
public:
    CTetrisData(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int shapeCount = 5);
    ~CTetrisData();

public:
    virtual bool InitGameData();
    bool ResetGameData();

public:
    CShape *GetCurrentShape();
    CShape *GetNextShape();
    void ChangeShape();
    void MergeCurrentShapeToBoard();
    void MergeNextShapeToBoard();
    int ClearFullLines();
    bool IsTopLineFilled();
    bool IsLineFull(const int nRow);
    bool IsLineEmpty(const int nRow);

protected:
    void InitAllShapes();
    void InitCurrentNextShape();
    void ClearPreviewBoard();

private:
    map<int, CShape* > m_allShapes;
    const int m_MAX_SHAPE_COUNT;
    int m_currentShapeKey;
    int m_nextShapeKey;
};


class CShapeState
{
public:
    /*
     * each shape state has 4 cells.
     * xp, yp of each cellPoint is the offset to shape position
     */
    QPoint m_cellPoint[4];
};

static const QColor g_ShapeColors[MAX_SHAPE_COLOR_COUNT] =
{
    Qt::black,
    Qt::darkGray,
    Qt::gray,
    Qt::lightGray,
    Qt::red,
    Qt::green,
    Qt::blue,
    Qt::cyan,
    Qt::magenta,
    Qt::yellow
};

class CShape
{
public:
    CShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount=4);
    virtual ~CShape();

    enum emMoveDirection{MD_LEFT, MD_RIGHT, MD_DOWN};

public:
    virtual bool InitShapeData();
    void ResetShape();

    bool MoveLeft();
    bool MoveRight();
    bool FallDown();
    bool Rotate();

    void MergeWithGameBoard();
    void MergeWithPreviewBoard();

    CShapeState *getCurrentState();
    QPoint *getShapePosition();
    QColor &getColor();


protected:
    CShapeState *getShapeState(const int stateKey);
    bool MoveOneCell(emMoveDirection direction);
    bool ChangeToNextState();

    void MergeCurrentStateWithGameBoard();
    void MergeCurrentStateWithPreviewBoard();

    /*
     * must called together
    */
    bool IsCellInPreviewBoard(int xpos, int ypos);
    bool IsCellInGameBoard(int xpos, int ypos);
    bool IsCellNotFilled(int xpos, int ypos);

    QColor GetFakeRandomColor();

protected:
    map<int, CShapeState* > m_allStates;
    int m_currentStateKey;
    QColor m_color;
    QPoint m_shapePosition;
    GAMEBOARD m_gameBoard;
    PREVIEWBOARD m_previewBoard;
    const int m_MAX_STATE_COUNT;
};

class CLineShape : public CShape
{
public:
    CLineShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount = 2);

public:
    bool InitShapeData();
};

class CSquareShape : public CShape
{
public:
    CSquareShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount =1 );

public:
    bool InitShapeData();
};

class CZ1Shape : public CShape
{
public:
    CZ1Shape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount = 2);

public:
    bool InitShapeData();
};

class CLShape : public CShape
{
public:
    CLShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount = 4);

public:
    bool InitShapeData();
};

class CWShape : public CShape
{
public:
    CWShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount = 4);

public:
    bool InitShapeData();
};


#endif // CTETRIS_H
