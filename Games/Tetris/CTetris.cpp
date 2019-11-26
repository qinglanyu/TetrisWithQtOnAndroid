#include "CTetris.h"
#include "gameglobal.h"

CTetris::CTetris()
{

}

CTetris::~CTetris()
{

}

bool CTetris::InitGame()
{
    ResetGrades();
    m_pTetrisData->MergeNextShapeToBoard();

    return true;
}

bool CTetris::RunGame()
{
    SetGameState(CGame::GS_Going);
    return true;
}

bool CTetris::PauseGame()
{
    SetGameState(CGame::GS_Pause);
    return true;
}

bool CTetris::ResetGame()
{
    SetGameState(CGame::GS_Ready);
    return true;
}

bool CTetris::ReleaseGame()
{
    SetGameState(CGame::GS_Over);
    return true;
}

bool CTetris::EndGame()
{
    SetGameState(CGame::GS_Over);
    return true;
}

bool CTetris::InitTheGameData(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard)
{
    m_pGameData = new CTetrisData(gameBoard, previewBoard);
    m_pTetrisData = dynamic_cast<CTetrisData*>(m_pGameData);
    return (m_pTetrisData != nullptr);
}

CShape *CTetris::GetMovingShape()
{
    return GetFallingShape();
}

bool CTetris::UpKeyDown()
{
#ifdef ANDROID
    return FloorKeyDown();
#else
    return RotateKeyDown();
#endif
}

bool CTetris::DownKeyDown()
{
    CShape *pShape = GetFallingShape();
    if(pShape)
    {
        pShape->FallDown();
    }

    return true;
}

bool CTetris::LeftKeyDown()
{
    CShape *pShape = GetFallingShape();
    if(pShape)
    {
        pShape->MoveLeft();
    }

    return true;
}

bool CTetris::RightKeyDown()
{
    CShape *pShape = GetFallingShape();
    if(pShape)
    {
        pShape->MoveRight();
    }

    return true;
}

bool CTetris::RotateKeyDown()
{
    CShape *pShape = GetFallingShape();
    if(pShape)
    {
        pShape->Rotate();
    }

    return true;
}

bool CTetris::FloorKeyDown()
{
    bool fell = FallingDownToBottom();
    if(false == fell)
        FinalEstimate();

    return fell;
}

bool CTetris::HeartMsgHandle()
{
    if(GS_Going != GetGameState())
    {
        return false;
    }

    // falling down one row
    bool fell = FallingOneRow();
    if(false == fell)
        FinalEstimate();

    return true;
}

void CTetris::FinalEstimate()
{
    UpdateGrades(ClearLines());
    IsGameOver();
}

bool CTetris::FallingOneRow()
{
    bool bMoved = true;
    CShape *pShape = GetFallingShape();
    Q_ASSERT(pShape);
    if(pShape)
    {
        bMoved = pShape->FallDown();
    }

    if(!bMoved)
    {
        // merge the falling shape when it reaches the bottom
        m_pTetrisData->MergeCurrentShapeToBoard();
        m_pTetrisData->ChangeShape();
        m_pTetrisData->MergeNextShapeToBoard();
    }

    return bMoved;
}

bool CTetris::FallingDownToBottom()
{
    bool bMoved = true;
    CShape *pShape = GetFallingShape();
    Q_ASSERT(pShape);
    do
    {
        bMoved = pShape->FallDown();
    }while(bMoved);

    m_pTetrisData->MergeCurrentShapeToBoard();
    m_pTetrisData->ChangeShape();
    m_pTetrisData->MergeNextShapeToBoard();

    return bMoved;
}

CShape *CTetris::GetFallingShape()
{
    if(m_pTetrisData)
        return m_pTetrisData->GetCurrentShape();
    else
        return nullptr;
}

int CTetris::ClearLines()
{
    return m_pTetrisData->ClearFullLines();
}

bool CTetris::IsGameOver()
{
    if(m_pTetrisData->IsTopLineFilled())
    {
        SetGameState(GS_Over);
    }

    return (GS_Over == GetGameState());
}

void CTetris::UpdateGrades(const int nCleardLines)
{
    m_score += (nCleardLines * nCleardLines * 100);
    m_level = (m_score/10000)%10;
}

void CTetris::ResetGrades()
{
    m_score = 0;
    m_level = 0;
}

CTetrisData::CTetrisData(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int shapeCount)
    : CGameData (gameBoard, previewBoard)
    , m_MAX_SHAPE_COUNT(shapeCount)
{
    InitGameData();
}

CTetrisData::~CTetrisData()
{
    map<int, CShape* >::iterator itor = m_allShapes.begin();
    while(m_allShapes.size() > 0)
    {
        CShape *pShape = itor->second;
        if(pShape)
        {
            delete pShape;
            itor = m_allShapes.erase(itor);
        }
    }
}

bool CTetrisData::InitGameData()
{
    static bool bInitialized = false;

    if(!bInitialized)
    {
        InitAllShapes();
        InitCurrentNextShape();

        bInitialized = true;
    }

    return bInitialized;
}

bool CTetrisData::ResetGameData()
{
    InitCurrentNextShape();

    return true;
}

CShape *CTetrisData::GetCurrentShape()
{
    map<int, CShape* >::iterator find = m_allShapes.find(m_currentShapeKey);
    if(find != m_allShapes.end())
        return find->second;
    else
        return nullptr;
}

CShape *CTetrisData::GetNextShape()
{
    map<int, CShape* >::iterator find = m_allShapes.find(m_nextShapeKey);
    if(find != m_allShapes.end())
        return find->second;
    else
        return nullptr;
}

void CTetrisData::ChangeShape()
{
    /*
     * reset current shape first and then
     * switch the current shape to the next one
     */
    CShape *pShape = GetCurrentShape();
    if(pShape)
        pShape->ResetShape();

    // change to the next one
    m_currentShapeKey = m_nextShapeKey;
    srand(unsigned(time(nullptr)));
    m_nextShapeKey = rand() % m_MAX_SHAPE_COUNT;
}

void CTetrisData::MergeCurrentShapeToBoard()
{
    CShape *pCurrentShape = GetCurrentShape();
    if(!pCurrentShape)
        return;

    pCurrentShape->MergeWithGameBoard();
}

void CTetrisData::MergeNextShapeToBoard()
{
    CShape *pNextShape = GetNextShape();
    if(!pNextShape)
        return;

    ClearPreviewBoard();

    pNextShape->MergeWithPreviewBoard();
}

int CTetrisData::ClearFullLines()
{
    int nClearLines = 0;
    for(int i=ROWCOUNT-1; i>=0; --i)    // from bottom to top lines
    {
        if(IsLineFull(i))
        {
            ++nClearLines;
            for(int m=i; m>=1; --m)
            {
                for(int jj=0; jj<COLCOUNT; ++jj)
                {
                    m_gameBoard[m][jj].FillCell(m_gameBoard[m-1][jj].IsFilled());
                    m_gameBoard[m][jj].SetColor(m_gameBoard[m-1][jj].GetColor());
                }
            }

            ++i;
        }
    }

    return nClearLines;
}

bool CTetrisData::IsTopLineFilled()
{
    return !IsLineEmpty(0);
}

bool CTetrisData::IsLineFull(const int nRow)
{
    bool bFull = true;
    for(int j=0; j<COLCOUNT; ++j)
    {
        if(!m_gameBoard[nRow][j].IsFilled())
        {
            bFull = false;
            break;
        }
    }

    return bFull;
}

bool CTetrisData::IsLineEmpty(const int nRow)
{
    bool bEmpty = true;
    for(int j=0; j<COLCOUNT; ++j)
    {
        if(m_gameBoard[nRow][j].IsFilled())
        {
            bEmpty = false;
            break;
        }
    }

    return bEmpty;
}

void CTetrisData::InitAllShapes()
{
    int shapeCount = 0;
    CShape *pShape = new CLineShape(m_gameBoard, m_previewBoard);
    pShape->InitShapeData();
    m_allShapes[shapeCount++] = pShape;

    pShape = new CSquareShape(m_gameBoard, m_previewBoard);
    pShape->InitShapeData();
    m_allShapes[shapeCount++] = pShape;

    pShape = new CZ1Shape(m_gameBoard, m_previewBoard);
    pShape->InitShapeData();
    m_allShapes[shapeCount++] = pShape;

    pShape = new CLShape(m_gameBoard, m_previewBoard);
    pShape->InitShapeData();
    m_allShapes[shapeCount++] = pShape;

    pShape = new CWShape(m_gameBoard, m_previewBoard);
    pShape->InitShapeData();
    m_allShapes[shapeCount++] = pShape;

    Q_ASSERT(shapeCount == m_MAX_SHAPE_COUNT);
}

void CTetrisData::InitCurrentNextShape()
{
    srand(unsigned(time(nullptr)));
    m_currentShapeKey = rand() % m_MAX_SHAPE_COUNT;
    m_nextShapeKey = rand() % m_MAX_SHAPE_COUNT;
}

void CTetrisData::ClearPreviewBoard()
{
    for(int ii=0; ii<4; ++ii)
    {
        for(int jj=0; jj<4; ++jj)
        {
            m_previewBoard[ii][jj].FillCell(false);
            m_previewBoard[ii][jj].SetColor(BOARD_COLOR);
        }
    }
}


CShape::CShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount)
    : m_gameBoard(gameBoard)
    , m_previewBoard(previewBoard)
    , m_MAX_STATE_COUNT(stateCount)
{

}

CShape::~CShape()
{
    map<int, CShapeState* >::iterator itor = m_allStates.begin();
    while(m_allStates.size() > 0)
    {
        CShapeState *pState = itor->second;
        if(pState)
        {
            delete pState;
            itor = m_allStates.erase(itor);
        }
    }
}

bool CShape::InitShapeData()
{
    ResetShape();
    return true;
}

void CShape::ResetShape()
{
    m_shapePosition = QPoint(0, COLCOUNT/2-1);
    srand(unsigned(time(nullptr)));
    m_currentStateKey = rand() % m_MAX_STATE_COUNT;

    //m_color = GetFakeRandomColor();
    m_color = Qt::black;
}

bool CShape::MoveLeft()
{
    return MoveOneCell(MD_LEFT);
}

bool CShape::MoveRight()
{
    return MoveOneCell(MD_RIGHT);
}

bool CShape::FallDown()
{
    return MoveOneCell(MD_DOWN);
}

bool CShape::Rotate()
{
    return ChangeToNextState();
}

void CShape::MergeWithGameBoard()
{
    MergeCurrentStateWithGameBoard();
}

void CShape::MergeWithPreviewBoard()
{
    MergeCurrentStateWithPreviewBoard();
}

void CShape::MergeCurrentStateWithGameBoard()
{
    CShapeState *pCurrentState = getCurrentState();
    if(!pCurrentState)
        return;

    for(int ii=0; ii<4; ++ii)
    {
        int xpos = pCurrentState->m_cellPoint[ii].x() + m_shapePosition.x();
        int ypos = pCurrentState->m_cellPoint[ii].y() + m_shapePosition.y();

        if(IsCellInGameBoard(xpos, ypos))
        {
            m_gameBoard[xpos][ypos].FillCell();
            m_gameBoard[xpos][ypos].SetColor(this->m_color);
        }
    }
}

void CShape::MergeCurrentStateWithPreviewBoard()
{
    CShapeState *pCurrentState = getCurrentState();
    if(!pCurrentState)
        return;

    for(int ii=0; ii<4; ++ii)
    {
        int xpos = pCurrentState->m_cellPoint[ii].x() + 1;
        int ypos = pCurrentState->m_cellPoint[ii].y() + 1;

        if(IsCellInPreviewBoard(xpos, ypos))
        {
            m_previewBoard[xpos][ypos].FillCell();
            m_previewBoard[xpos][ypos].SetColor(this->m_color);
        }
    }
}

bool CShape::IsCellInPreviewBoard(int xpos, int ypos)
{
    return ((0<=xpos) && (xpos<PRVBCOUNT) && (0<=ypos) && (ypos<PRVBCOUNT));
}

CShapeState *CShape::getCurrentState()
{
    return getShapeState(m_currentStateKey);
}

QPoint *CShape::getShapePosition()
{
    return &m_shapePosition;
}

QColor &CShape::getColor()
{
    return m_color;
}

CShapeState *CShape::getShapeState(const int stateKey)
{
    map<int, CShapeState* >::iterator find = m_allStates.find(stateKey);
    if(find != m_allStates.end())
    {
        return find->second;
    }
    else
    {
        return nullptr;
    }
}

bool CShape::MoveOneCell(CShape::emMoveDirection direction)
{
    bool bMoved = true;

    static QPoint offsetTbl[3] = {QPoint(0, -1), QPoint(0, 1), QPoint(1, 0)}; // left, right, down
    QPoint offset = offsetTbl[direction];

    CShapeState *pCurntState = getCurrentState();
    for(int ii=0; ii<4; ++ii)
    {
        QPoint movedPoint = pCurntState->m_cellPoint[ii] + m_shapePosition + offset;
        int xpos = movedPoint.x();
        int ypos = movedPoint.y();

        if(xpos<0 && (0<=ypos && ypos <COLCOUNT))
            continue;

        if(!IsCellNotFilled(xpos, ypos))
        {
            bMoved = false;
            break;
        }
    }

    if(bMoved)
    {
        m_shapePosition += offset;
    }

    return bMoved;
}

bool CShape::ChangeToNextState()
{
    bool bChanged = true;
    int maxStateCount = int(m_allStates.size());
    int nextStateKey = (m_currentStateKey+1)%maxStateCount;

    CShapeState *pNextState = getShapeState(nextStateKey);
    if(nullptr == pNextState)
        return false;

    for(int ii=0; ii<4; ++ii)
    {
        QPoint changedPoint = pNextState->m_cellPoint[ii] + m_shapePosition;
        int xpos = changedPoint.x();
        int ypos = changedPoint.y();

        if(xpos<0)
            continue;

        if(!IsCellNotFilled(xpos, ypos))
        {
            bChanged = false;
            break;
        }
    }

    if(bChanged)
    {
        m_currentStateKey = nextStateKey;
    }
    return bChanged;
}

bool CShape::IsCellInGameBoard(int xpos, int ypos)
{
    return ((0<=xpos) && (xpos<ROWCOUNT) && (0<=ypos) && (ypos<COLCOUNT));
}

bool CShape::IsCellNotFilled(int xpos, int ypos)
{
    return (IsCellInGameBoard(xpos, ypos) && !m_gameBoard[xpos][ypos].IsFilled());
}

QColor CShape::GetFakeRandomColor()
{
    QColor color;
    do
    {
        int nColorIdx = rand()%MAX_SHAPE_COLOR_COUNT;
        color = g_ShapeColors[nColorIdx];
    }
    while(color == Qt::black);

    return color;
}


CLineShape::CLineShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount)
    : CShape (gameBoard, previewBoard, stateCount)
{

}

bool CLineShape::InitShapeData()
{
    int stateCount = 0;
    CShapeState *pState = new CShapeState;
    pState->m_cellPoint[0].setY(-1);
    pState->m_cellPoint[1].setY(0);
    pState->m_cellPoint[2].setY(1);
    pState->m_cellPoint[3].setY(2);
    m_allStates[stateCount++] = pState;

    pState = new CShapeState;
    pState->m_cellPoint[0] = QPoint(-1, 1);
    pState->m_cellPoint[1] = QPoint(0, 1);
    pState->m_cellPoint[2] = QPoint(1, 1);
    pState->m_cellPoint[3] = QPoint(2, 1);
    m_allStates[stateCount++] = pState;

    Q_ASSERT(stateCount == m_MAX_STATE_COUNT);

    return CShape::InitShapeData();
}

CSquareShape::CSquareShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount)
    : CShape(gameBoard, previewBoard, stateCount)
{

}

bool CSquareShape::InitShapeData()
{
    int stateCount = 0;
    CShapeState *pState = new CShapeState;
    pState->m_cellPoint[1] = QPoint(1, 0);
    pState->m_cellPoint[2] = QPoint(0, 1);
    pState->m_cellPoint[3] = QPoint(1, 1);
    m_allStates[stateCount++] = pState;

    Q_ASSERT(stateCount == m_MAX_STATE_COUNT);
    return CShape::InitShapeData();
}

CZ1Shape::CZ1Shape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount)
    : CShape(gameBoard, previewBoard, stateCount)
{

}

bool CZ1Shape::InitShapeData()
{
    int stateCount = 0;
    CShapeState *pState = new CShapeState;
    pState->m_cellPoint[1] = QPoint(0, 1);
    pState->m_cellPoint[2] = QPoint(1, 1);
    pState->m_cellPoint[3] = QPoint(1, 2);
    m_allStates[stateCount++] = pState;

    pState = new CShapeState;
    pState->m_cellPoint[0] = QPoint(0, 1);
    pState->m_cellPoint[1] = QPoint(1, 1);
    pState->m_cellPoint[2] = QPoint(0, 2);
    pState->m_cellPoint[3] = QPoint(-1, 2);
    m_allStates[stateCount++] = pState;

    Q_ASSERT(stateCount == m_MAX_STATE_COUNT);
    return CShape::InitShapeData();
}

CLShape::CLShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount)
    : CShape (gameBoard, previewBoard, stateCount)
{

}

bool CLShape::InitShapeData()
{
    int stateCount = 0;
    CShapeState *pState = new CShapeState;
    pState->m_cellPoint[1] = QPoint(0, 1);
    pState->m_cellPoint[2] = QPoint(0, 2);
    pState->m_cellPoint[3] = QPoint(1, 2);
    m_allStates[stateCount++] = pState;

    pState = new CShapeState;
    pState->m_cellPoint[0] = QPoint(0, 1);
    pState->m_cellPoint[1] = QPoint(1, 1);
    pState->m_cellPoint[2] = QPoint(-1, 1);
    pState->m_cellPoint[3] = QPoint(-1, 2);
    m_allStates[stateCount++] = pState;

    pState = new CShapeState;
    pState->m_cellPoint[1] = QPoint(-1, 0);
    pState->m_cellPoint[2] = QPoint(0, 1);
    pState->m_cellPoint[3] = QPoint(0, 2);
    m_allStates[stateCount++] = pState;

    pState = new CShapeState;
    pState->m_cellPoint[0] = QPoint(0, 1);
    pState->m_cellPoint[1] = QPoint(-1, 1);
    pState->m_cellPoint[2] = QPoint(1, 1);
    pState->m_cellPoint[3] = QPoint(1, 0);
    m_allStates[stateCount++] = pState;

    Q_ASSERT(stateCount == m_MAX_STATE_COUNT);
    return CShape::InitShapeData();
}

CWShape::CWShape(GAMEBOARD gameBoard, PREVIEWBOARD previewBoard, const int stateCount)
    : CShape(gameBoard, previewBoard, stateCount)
{

}

bool CWShape::InitShapeData()
{
    int stateCount = 0;
    CShapeState *pState = new CShapeState;
    pState->m_cellPoint[1] = QPoint(-1, 1);
    pState->m_cellPoint[2] = QPoint(0, 1);
    pState->m_cellPoint[3] = QPoint(1, 1);
    m_allStates[stateCount++] = pState;

    pState = new CShapeState;
    pState->m_cellPoint[1] = QPoint(0, 1);
    pState->m_cellPoint[2] = QPoint(0, 2);
    pState->m_cellPoint[3] = QPoint(1, 1);
    m_allStates[stateCount++] = pState;

    pState = new CShapeState;
    pState->m_cellPoint[0] = QPoint(-1, 1);
    pState->m_cellPoint[1] = QPoint(0, 1);
    pState->m_cellPoint[2] = QPoint(0, 2);
    pState->m_cellPoint[3] = QPoint(1, 1);
    m_allStates[stateCount++] = pState;

    pState = new CShapeState;
    pState->m_cellPoint[1] = QPoint(0, 1);
    pState->m_cellPoint[2] = QPoint(0, 2);
    pState->m_cellPoint[3] = QPoint(-1, 1);
    m_allStates[stateCount++] = pState;

    Q_ASSERT(stateCount == m_MAX_STATE_COUNT);
    return CShape::InitShapeData();
}
