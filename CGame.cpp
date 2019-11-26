#include "CGame.h"

CGame::CGame()
    : m_pGameData(nullptr)
    , m_score(0)
    , m_level(1)
{

}

CGame::~CGame()
{

}

int CGame::GetLevel() const
{
    return m_level;
}

int CGame::GetScore() const
{
    return m_score;
}

void CGame::SetGameState(CGame::emGameState gs)
{
    m_gameState = gs;
}

CGame::emGameState CGame::GetGameState() const
{
    return m_gameState;
}



CGameData::CGameData(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard)
    : m_gameBoard(gameBoard)
    , m_previewBoard(previewBoard)
{

}

CGameData::~CGameData()
{

}
