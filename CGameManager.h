#ifndef CGAMEMANAGER_H
#define CGAMEMANAGER_H

#include <QObject>
#include <QMutex>

#include "CGame.h"

class CGame;
class CCell;
class QTimer;
class CShape;

typedef CCell **GAMEBOARD, **PREVIEWBOARD;

class CGameManager : public QObject
{    
    Q_OBJECT

public:
    CGameManager();
    ~CGameManager();

    enum emGameAction
    {
        GA_NewGame,
        GA_Begin,
        GA_Stop,
        GA_Restart
    };

    enum emMoveDirection
    {
        MD_Up,
        MD_Down,
        MD_Left,
        MD_Right,
        MD_Rotate,
        MD_Floor
    };

public:
    bool InitGameBoard();
    bool InitPreviewBoard();

    int GetScore();
    int GetLevel();
    int GetSpeed();

    GAMEBOARD GetGameBoard();
    int GetGameBoardRowCount() const;
    int GetGameBoardColCount() const;

    PREVIEWBOARD GetPreviewBoard();
    int GetPreviewBoardSize() const;

    CShape *GetMovingShape();
    CGame::emGameState GetGameState();

protected:
    bool ResetGame();
    bool ResetGameBoard();
    bool ResetPreviewBoard();

    void NewGame();
    void BeginGame();
    void StopGame();
    void RestartGame();



Q_SIGNALS:
    void sigUpdateUI();
    void sigStartGame(int nIndex);

public Q_SLOTS:
    void slotMoveDirection(int/*emMoveDirection*/ moveDirection);
    void slotGameAction(int/*emGameAction*/ gameAction);
    void slotHeartHandler();
    void slotInitGameManager(QPoint gbTopLeft, QPoint pbTopLeft, int gbWidth, int gbHeight);

private:
    int m_score;
    int m_level;
    int m_speed;
    int m_periodMs;

    QTimer *m_heartTimer;

    GAMEBOARD m_pGameBoard;
    PREVIEWBOARD m_pPreviewBoard;
    QPoint m_gmbdTopLeft, m_prbdTopLeft;
    int m_gmbdWidth;
    int m_gmbdHeight;

    CGame *m_pGame;

    QMutex m_mutex;
};

#endif // CGAMEMANAGER_H
