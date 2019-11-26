#include "CGameManager.h"
#include "gameglobal.h"
#include "CCell.h"
#include "CGame.h"
#include "./Games/Tetris/CTetris.h"

#include <QDebug>
#include <QTimer>
#include <QPoint>

CGameManager::CGameManager()
    : QObject(nullptr)
    , m_speed(5)
    , m_heartTimer(nullptr)
    , m_pGameBoard(nullptr)
    , m_pPreviewBoard(nullptr)
    , m_pGame(nullptr)
{

}

CGameManager::~CGameManager()
{
    if(nullptr != m_pGameBoard)
    {
        for(int ii=0; ii<ROWCOUNT; ++ii)
        {
            delete m_pGameBoard[ii];
            m_pGameBoard[ii] = nullptr;
        }

        delete m_pGameBoard;
        m_pGameBoard = nullptr;
    }

    if(nullptr != m_pPreviewBoard)
    {
        for(int ii=0; ii<PRVBCOUNT; ++ii)
        {
            delete m_pPreviewBoard[ii];
            m_pPreviewBoard[ii] = nullptr;
        }

        delete m_pPreviewBoard;
        m_pPreviewBoard = nullptr;
    }
}

bool CGameManager::InitGameBoard()
{
    Q_ASSERT(nullptr == m_pGameBoard);
    int gameboard_L = m_gmbdTopLeft.x()+(m_gmbdWidth%COLCOUNT)/2;
    int gameboard_T = m_gmbdTopLeft.y()+(m_gmbdHeight%ROWCOUNT)/2;
    int cellsize = m_gmbdWidth/COLCOUNT;

    m_pGameBoard = static_cast<GAMEBOARD>(new CCell*[ROWCOUNT]);
    for (int ii=0; ii<ROWCOUNT; ++ii)
    {
        m_pGameBoard[ii] = static_cast<CCell*>(new CCell[COLCOUNT]);

        for (int jj=0; jj<COLCOUNT; ++jj)
        {
            CCell cell(gameboard_L+jj*(cellsize), gameboard_T+ii*(cellsize), cellsize, cellsize);
            m_pGameBoard[ii][jj] = cell;
        }
    }

    return true;
}

bool CGameManager::InitPreviewBoard()
{
    Q_ASSERT(nullptr == m_pPreviewBoard);

    int previewboard_L = m_prbdTopLeft.x();
    int previewboard_T = m_prbdTopLeft.y();
    int cellsize = m_gmbdWidth/COLCOUNT;

    m_pPreviewBoard = static_cast<PREVIEWBOARD>(new CCell*[PRVBCOUNT]);
    for (int ii=0; ii<PRVBCOUNT; ++ii)
    {
        m_pPreviewBoard[ii] = static_cast<CCell*>(new CCell[PRVBCOUNT]);

        for (int jj=0; jj<PRVBCOUNT; ++jj)
        {
            CCell cell(previewboard_L + jj*cellsize, previewboard_T + ii*cellsize, cellsize, cellsize);
            m_pPreviewBoard[ii][jj] = cell;
        }
    }

    return true;
}

int CGameManager::GetScore()
{
    QMutexLocker locker(&m_mutex);
    return (m_pGame ? m_pGame->GetScore() : 0);
}

int CGameManager::GetLevel()
{
    QMutexLocker locker(&m_mutex);
    return (m_pGame ? m_pGame->GetLevel() : 1);
}

int CGameManager::GetSpeed()
{
    QMutexLocker locker(&m_mutex);
    return m_speed;
}

GAMEBOARD CGameManager::GetGameBoard()
{
    QMutexLocker locker(&m_mutex);
    return m_pGameBoard;
}

int CGameManager::GetGameBoardRowCount() const
{
    return ROWCOUNT;
}

int CGameManager::GetGameBoardColCount() const
{
    return COLCOUNT;
}

int CGameManager::GetPreviewBoardSize() const
{
    return PRVBCOUNT;
}

CShape *CGameManager::GetMovingShape()
{
    QMutexLocker locker(&m_mutex);

    if(m_pGame != nullptr)
    {
        return m_pGame->GetMovingShape();
    }
    else
    {
        return nullptr;
    }
}

CGame::emGameState CGameManager::GetGameState()
{
    if(m_pGame)
        return m_pGame->GetGameState();
    else
        return CGame::GS_Over;
}

bool CGameManager::ResetGame()
{
    ResetGameBoard();
    ResetPreviewBoard();
    return true;
}

bool CGameManager::ResetGameBoard()
{
    Q_ASSERT(nullptr != m_pGameBoard);

    for (int ii=0; ii<ROWCOUNT; ++ii)
    {
        for (int jj=0; jj<COLCOUNT; ++jj)
        {
            m_pGameBoard[ii][jj].SetColor(BOARD_COLOR);
            m_pGameBoard[ii][jj].FillCell(false);
        }
    }

    return true;
}

bool CGameManager::ResetPreviewBoard()
{
    Q_ASSERT(nullptr != m_pPreviewBoard);

    for (int ii=0; ii<PRVBCOUNT; ++ii)
    {
        for (int jj=0; jj<PRVBCOUNT; ++jj)
        {
            m_pPreviewBoard[ii][jj].SetColor(BOARD_COLOR);
            m_pPreviewBoard[ii][jj].FillCell(false);
        }
    }

    return true;
}

void CGameManager::NewGame()
{
    m_pGame = new CTetris();
    m_pGame->InitTheGameData(m_pGameBoard, m_pPreviewBoard);
    m_pGame->InitGame();

    emit sigStartGame(0);
}

void CGameManager::BeginGame()
{
    if(nullptr == m_pGame)
        return;

    m_pGame->RunGame();
    m_periodMs = 1000 - 10 * m_pGame->GetLevel();
    m_heartTimer->start(m_periodMs);
}

void CGameManager::StopGame()
{
    if(nullptr == m_pGame)
        return;

    m_heartTimer->stop();
    m_pGame->PauseGame();
}

void CGameManager::RestartGame()
{
    ResetGame();
    BeginGame();
}

void CGameManager::slotMoveDirection(int/*CGameManager::emMoveDirection*/ moveDirection)
{
    if(nullptr == m_pGame)
        return;

    if(CGame::GS_Going != m_pGame->GetGameState())
    {
        if(moveDirection == MD_Rotate)
        {
            BeginGame();
        }
        return;
    }

    switch (moveDirection)
    {
    case MD_Up:
        m_pGame->UpKeyDown();
        break;
    case MD_Down:
        m_pGame->DownKeyDown();
        break;
    case MD_Left:
        m_pGame->LeftKeyDown();
        break;
    case MD_Right:
        m_pGame->RightKeyDown();
        break;
    case MD_Rotate:
        m_pGame->RotateKeyDown();
        break;
    case MD_Floor:
        m_pGame->FloorKeyDown();
        break;
    default:
        break;
    }

    emit sigUpdateUI();
}

void CGameManager::slotGameAction(int/*CGameManager::emGameAction*/ gameAction)
{
    if((gameAction == GA_NewGame) && (m_pGame != nullptr))
    {
        CGame::emGameState gs = m_pGame->GetGameState();

        switch (gs)
        {
        case CGame::GS_Going:
            StopGame(); return;
        case CGame::GS_Pause:
            BeginGame(); return;
        default:
            break;
        }
    }


    switch(gameAction)
    {
    case GA_NewGame:
        NewGame();
        break;
    case GA_Begin:
        BeginGame();
        break;
    case GA_Stop:
        StopGame();
        break;
    case GA_Restart:
        RestartGame();
        break;
    default:
        break;
    }
}

void CGameManager::slotHeartHandler()
{
    if(m_pGame == nullptr)
        return;

    m_pGame->HeartMsgHandle();

    emit sigUpdateUI();
}

void CGameManager::slotInitGameManager(QPoint gbTopLeft, QPoint pbTopLeft, int gbWidth, int gbHeight)
{
    m_gmbdTopLeft = gbTopLeft;
    m_prbdTopLeft = pbTopLeft;
    m_gmbdWidth = gbWidth;
    m_gmbdHeight = gbHeight;

    if(nullptr == m_heartTimer)
    {
        m_heartTimer = new QTimer();
        bool bRet = connect(m_heartTimer, SIGNAL(timeout()), this, SLOT(slotHeartHandler()));
        if(!bRet) qDebug() << "connect signal and slot failed." << __FILE__ << __LINE__ << endl;
    }

    InitGameBoard();
    InitPreviewBoard();
}


PREVIEWBOARD CGameManager::GetPreviewBoard()
{
    QMutexLocker locker(&m_mutex);
    return m_pPreviewBoard;
}
