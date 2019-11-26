#ifndef CGAME_H
#define CGAME_H

#include "CCell.h"
typedef CCell **GAMEBOARD, **PREVIEWBOARD;
#define BOARD_COLOR QColor("#899875")

class CGameData;
class CShape;

class CGame
{
public:
    CGame();
    virtual ~CGame();

    enum emGameState
    {
        GS_Ready,   // ready to begin
        GS_Going,   // game running
        GS_Pause,   // game paused
        GS_Over     // game end
    };

public:
    virtual bool InitGame() = 0;
    virtual bool RunGame() = 0;
    virtual bool PauseGame() = 0;
    virtual bool ResetGame() = 0;   // reset game before start
    virtual bool ReleaseGame() = 0; // continue game after paused
    virtual bool EndGame() = 0;

    virtual bool InitTheGameData(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard) = 0;
    virtual CShape* GetMovingShape() = 0;   // the moving shape object in games

public:
    virtual bool UpKeyDown() = 0;
    virtual bool DownKeyDown() = 0;
    virtual bool LeftKeyDown() = 0;
    virtual bool RightKeyDown() = 0;
    virtual bool RotateKeyDown() = 0;
    virtual bool FloorKeyDown() = 0;    // space key
    virtual bool HeartMsgHandle() = 0;

    virtual int GetLevel() const;
    virtual int GetScore() const;

    emGameState GetGameState() const;

protected:
    void SetGameState(emGameState gs);

    virtual bool IsGameOver() = 0;
    virtual void UpdateGrades(const int nCleardLines) = 0;
    virtual void ResetGrades() = 0;

protected:
    CGameData *m_pGameData;
    emGameState m_gameState;

    int m_score;        // game score
    int m_level;        // game level [1-9]
};

class CGameData
{
public:
    CGameData(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard);
    virtual ~CGameData();
    virtual bool InitGameData() = 0;

protected:
    GAMEBOARD m_gameBoard;
    PREVIEWBOARD m_previewBoard;
};

#endif // CGAME_H
