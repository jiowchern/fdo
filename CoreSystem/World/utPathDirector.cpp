#include "stdafx.h"
#include "utpathdirector.h"

short utPathDirector::incx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
short utPathDirector::incy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
char utPathDirector::inc2r[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

const BYTE utPathDirector::m_iSeqNoTH = 4;
const BYTE utPathDirector::m_iMaxSeqNo = 8;

// 方向代號圖解
// ┌─┬─┬─┐
// │５│４│３│
// ├─┼─┼─┤
// │６│＊│２│
// ├─┼─┼─┤
// │７│０│１│
// └─┴─┴─┘
// 以目前incx, incy計算出來的inc2r陣列
//char utPathDirector::inc2r[9] = {7, 0, 1, 6, 8, 2, 5, 4, 3};

utPathDirector::utPathDirector(void)
{
	//_Sender = NULL;
    Node = NULL;
    Open = NULL;
    Close = NULL;
    Modify = NULL;
}

utPathDirector::~utPathDirector(void)
{
}

// 待處理節點入佇列, 依靠對目的地估價距離插入排序
char utPathDirector::AddToOpenQueue(TAstarNode *node)
{
    ADWORD i;
    short f = node->EstimateCost;
    TAstarNode *p, *b;

    node->Modified |= 2;    // 記錄Open標誌

	// Open Queue中節點依估計價排序，由小到大，最後一個為起始點，
	// 等價Node則越晚進，排越前面
    for (b = NULL, p = Open, i = 0; p && (i <= OpenCount); b = p, p = p->Next, i++) 
        if (f <= p->EstimateCost) break;
	// 若此節點估計價大於起始點，排除之
    if (i > OpenCount) return -2;
	// 將此節點插入b跟p之間
    node->Next = p;
    if (b)
		b->Next = node;
    else 
		Open = node;
	// 遞增Open Queue計數
    OpenCount++;
    return 0;
}

// 將離目的地估計最近的方案出佇列
char utPathDirector::GetFromOpenQueue()
{
    TAstarNode *BestChoice=Open;

    if (!Open)
		return -1;
    Open = Open->Next;
	// 最小點已經在Close Queue中了，排除之
	if (BestChoice->Modified & 4)
		return -2;
	// 將此節點加到Close Queue的最前面
    BestChoice->Next = Close;
    BestChoice->Modified &= 5;    // 清除Open標誌
    BestChoice->Modified |= 4;    // 設置Close標誌
    Close = BestChoice;

    OpenCount--;
    CloseCount++;
    return 0;
}

// Pop Close Stack節點
char utPathDirector::PopCloseStack()
{
    if (Close)
	{ 
        Close->Modified &= 3;        // 清除Close標誌
        Close = Close->Next;
        CloseCount--; 
        return 0; 
    }
    return -1;
}

// 還原修改過的所有節點
void utPathDirector::ClearModified()
{
    ADWORD i;
    for (i = 0; i < ModifyPoint; i++) 
	{
        Modify[i]->Modified = 0;
        Modify[i]->ActualCost = COST_MAX;
    }
    ModifyPoint = 0;
    OpenCount = 0;
    CloseCount = 0;
    Open = NULL;
    Close = NULL;
}

// 嘗試下一步移動到 x,y 可行否
char utPathDirector::TryTile(short x, short y, short endx, short endy, TAstarNode *Father, BYTE Dir)
{
    int ActualCost;
	//short PreX, PreY;

	// 如果地圖無法通過則退出
    if (!mPossible(x, y))
		return 1;           
    if (Node[y][x].Modified & 6) return 1;		// 如果已經存在在Open,Close佇列中則退出

	// 斜角距離為1格算時以下列加權計算
#if (OBLIQUE_ONE == 1)
	ActualCost = Father->ActualCost + 1;	// 實際值計算
#else
	// 斜角距離不以1格算時以下列加權計算
	if ((abs(x - endx) == 1) && (abs(y - endy) == 1))
		ActualCost = Father->ActualCost + 14;	// 斜角以實際值的√2(1.4142135)計算
	else
		ActualCost = Father->ActualCost + 10;	// 實際值計算
#endif //(OBLIQUE_ONE == 1)

    if (ActualCost >= Node[y][x].ActualCost)
		return 1;
    if (!Node[y][x].Modified)
		Modify[ModifyPoint++] = &Node[y][x];	// 記錄這個修改過的點以還原
    Node[y][x].ActualCost = (short)ActualCost;
    Node[y][x].Modified = 1;
    Node[y][x].Father = Father;					// 將路徑點從終點向起點鏈結起來
	Node[y][x].EstimateCost = (short)ActualCost + Judge(x, y, endx, endy, Father->Dir, Dir);
	Node[y][x].Dir = Dir;
    AddToOpenQueue(&Node[y][x]);				// 將節點加入Open佇列
    return 0;
}

// 路徑尋找主函數；回傳值：-2: Close Queue已空 -1: Open Queue已空 1: 可到達終點 2: 路徑長度超過設定之最小值
int utPathDirector::FindPath(short startx, short starty, short endx, short endy)
{
    TAstarNode *root;
    int i, j, x, y, ok, dirs, max = 0;
    short MinJudge;

    if (!Node || !Modify) return -1;
    ClearModified();
    root = &Node[starty][startx];
    root->ActualCost = 0;
    root->EstimateCost = Judge(startx, starty, endx, endy, 0, 0);
    root->Father = NULL; 
    root->Modified = 1;
	root->Dir = 0;
    Modify[ModifyPoint++] = root;
	// 將根節點﹝起始點﹞加入Open Queue
    AddToOpenQueue(root);
	// 將起始點轉存成(16, 16)模式
    MinPos = tile_pos(startx, starty);
	// MinJudge的初始值設為初始點跟終點的估計開銷
    //MinJudge = Judge(startx, starty, endx, endy);
    MinJudge = root->EstimateCost;
    for (ok = 0; ok == 0; )
    {
        ok = GetFromOpenQueue();      // 取出Open佇列估價值最小的節點放入Close中
        root = Close;                 // 得到剛才取出的節點
		// 若Open Queue中已無節點，或是取出點已經在Close Queue中，中斷之
        if (ok || root == NULL)
		{
			ok = -1; 
			break; 
		}
		// 取得此節點的座標值
        x = tile_x(root->Pos);
        y = tile_y(root->Pos);
		// 找估計離終點最近的點；若此節點估計開銷減掉實際開銷﹝即估算函式算出值﹞比最小估計開銷小，修正最小開銷值為此節點
        if (root->EstimateCost - root->ActualCost < MinJudge)
		{
            MinJudge = root->EstimateCost - root->ActualCost;
			MinPos = root->Pos;
		}
        if (CloseCount > (ADWORD)max)
			max = CloseCount;
        if ((x == endx) && (y == endy))
		{// 如果已達終點
			MinPos = root->Pos;
			ok = 1;
		}
        else
		{// 如果未達終點
			// 向八方向進行測試
            for (i = 0, dirs = 1, j = 1; i < 8; i++, dirs <<= 1)
                if (_DirMask & dirs) 
					j &= TryTile((short)x + incx[i], (short)y + incy[i], endx, endy, root, i);

            if (j)
			{// 若八方向全部不可走，自Close Queue中Pop一個元素
				if (PopCloseStack())
				{// Close Queue中已空
					ok = -2;
					break;
				}   // 如果不是通路則從Close中取出
			}
        }
        if (OpenCount >= _OpenLimited || CloseCount >= _CloseLimited)
		{//超出範圍時紀錄最後一點
			MinPos = root->Pos;
			ok = 2;
		}
    }
  //  if (ok < 0) 
		//return -2;
  //  return 0;
	return ok;
}

int utPathDirector::GetPosPath(short *PosXY, int *MaxPosCount, short *SimpPosXY, int *SimpMaxPosCount, short KeepBackDistance)
{
    short X, Y, NewX, NewY;
    ADWORD /*Pos,*/j;
    TAstarNode *p, *s;
    int i , idxPos;
	char PosDir, PrePosDir = 0;
	int k;
	short *ptSimpPos;
	short MaxDistance = -1;
	short ActualCost;

    if (*MaxPosCount > 1)
		(*MaxPosCount)--;

    X = (short)tile_x(MinPos);
    Y = (short)tile_y(MinPos);
    for (p = &Node[Y][X], s = p, j = 0, k = 0; p && j < MaxSize; p = p->Father, j++)
    {
        NewX = (short)tile_x(p->Pos);
        NewY = (short)tile_y(p->Pos);
#if (OBLIQUE_ONE == 1)
		ActualCost = p->ActualCost;
#else
		ActualCost = p->ActualCost / 10;
#endif //(OBLIQUE_ONE == 1)
		if (ActualCost <= (*MaxPosCount))
		{
			if (MaxDistance == -1)
				MaxDistance = ActualCost - KeepBackDistance;
	        i = ActualCost;
			idxPos = i << 1;
			PosXY[idxPos] = NewX;
			PosXY[idxPos + 1] = NewY;
			PosDir = inc2r[(Y - NewY + 1) * 3 + X - NewX + 1];
			if (i <= MaxDistance)
			{
				//if (((k == 0) /*&& (PosDir != 8)*/) || (i == 0))
				if ((k == 0) || (i == 0))
				{// 記頭尾兩點
					_arrSimpPos[k] = PosXY + idxPos;
					k++;
				}
				else if ((PosDir != PrePosDir) && (PrePosDir != 8))
				{// 中間轉折點
					_arrSimpPos[k] = PosXY + ((i + 1) << 1);
					k++;
				}
			}
			PrePosDir = PosDir;
		}
        X = NewX;
        Y = NewY;
    }
#if (OBLIQUE_ONE == 1)
	ActualCost = s->ActualCost + 1;
#else
	ActualCost = s->ActualCost / 10 + 1;
#endif //(OBLIQUE_ONE == 1)
	i = (ActualCost < (*MaxPosCount)) ? ActualCost : (*MaxPosCount);
	idxPos = i << 1;
    PosXY[idxPos] = -1;
    PosXY[idxPos + 1] = -1;
	(*MaxPosCount) = i;
	(*SimpMaxPosCount) = k;

	for (k = ((*SimpMaxPosCount) - 1), i = 0; k >= 0; k--, i++)
	{
		ptSimpPos = _arrSimpPos[k];
		idxPos = i << 1;
		SimpPosXY[idxPos] = *ptSimpPos;
		SimpPosXY[idxPos + 1] = *(ptSimpPos + 1);
	}
    return 0;
}

int utPathDirector::GetDirPath(char *PosDir, int MaxPosCount)
{
    short i, X, Y, NewX, NewY;
    ADWORD j;
    TAstarNode *p, *s;
	short ActualCost;

   // if (!inc2r[0])
   // {
   //     for (i = 0; i < 8; i++) 
			//inc2r[(incy[i] + 1) * 3 + incx[i] + 1] = (char)i;
   //     inc2r[4] = 8;
   // }
    X = (short)tile_x(MinPos);
    Y = (short)tile_y(MinPos);
    if (MaxPosCount > 1)
		MaxPosCount--;
    for (p = &Node[Y][X], s = p, j = 0; p && j < MaxSize; p = p->Father, j++)
    {
        NewX = (short)tile_x(p->Pos);
        NewY = (short)tile_y(p->Pos);
#if (OBLIQUE_ONE == 1)
        i = (p->ActualCost < MaxPosCount) ? p->ActualCost : (short)MaxPosCount;
#else
		ActualCost = p->ActualCost / 10;
		i = (ActualCost < MaxPosCount) ? ActualCost : (short)MaxPosCount;
#endif //(OBLIQUE_ONE == 1)
        PosDir[i] = inc2r[(Y - NewY + 1) * 3 + X - NewX + 1];
        X = NewX;
        Y = NewY;
    }
#if (OBLIQUE_ONE == 1)
	ActualCost = s->ActualCost + 1;
#else
	ActualCost = s->ActualCost / 10 + 1;
#endif //(OBLIQUE_ONE == 1)
    i = (ActualCost < MaxPosCount) ? ActualCost : (short)MaxPosCount;
    PosDir[i] = 8;
    return 0;
}

int utPathDirector::Initialize(int MapWidth, int MapHeight, WORD MaxDistance)
{
    int i, j;

	if ((_arrSimpPos = new short *[MaxDistance]) != NULL)
	{
		height = MapHeight;
		width = MapWidth;
		MaxSize = MapHeight * MapWidth;
		for (i = 0; i < 8; i++) 
			inc2r[(incy[i] + 1) * 3 + incx[i] + 1] = (char)i;
		inc2r[4] = 8;
		Modify = new TAstarNode*[MaxSize];
		if (!Modify)
			return -1;
		Node = new TAstarNode*[MapHeight];
		if (!Node)
		{
			delete Modify;
			Modify = NULL;
			return -1;
		}
		for (i = 0; i < MapHeight; i++)
			Node[i] = new TAstarNode[MapWidth];
		for (i = 0, j = 1; i < MapHeight; i++)
			if (!Node[i])
				j = 0;
		if (!j)
		{
			for (i = 0; i < MapHeight; i++)
				if (Node[i]) 
					delete Node[i];
			delete Node;
			delete Modify;
			Node = NULL;
			Modify = NULL;
			return -2;
		}
		for (j = 0;j < MapHeight; j++) 
			for (i = 0; i < MapWidth; i++) 
			{
				Node[j][i].Pos = tile_pos(i, j);
				Node[j][i].Modified = 0;
				Node[j][i].ActualCost = COST_MAX;
			}
		ModifyPoint = 0;
		//SetMapCheck(MoveCheck);
		//SetJudgeFun(JudgeCost = JudgeFun);
		SetDirMask(0x55);
		SetOpenLimited(200);
		SetCloseLimited(MaxSize/20);

		//_treOpen.Initialize(MaxSize);
	}

    return 0;
}
int utPathDirector::Destroy()
{
    int /*i, */j;

	//_treOpen.Destroy();

    if (Node) for (j = 0;j<height;j++) if (Node[j]) delete [] Node[j];
    if (Node) delete [] Node;
    if (Modify) delete [] Modify;
    Node = NULL;
    Modify = NULL;
    return 0;
}

//void utPathDirector::SetJudgeFun(ShortSSFunc JudgeFun) 
//{ 
//	JudgeCost = JudgeFun;
//}
//
//void utPathDirector::SetMapCheck(CharSSFunc MapCheck)
//{ 
//	Moveable = MapCheck;
//}

void utPathDirector::SetOpenLimited(long OpenLimited)
{ 
	_OpenLimited = OpenLimited;
}

void utPathDirector::SetCloseLimited(long CloseLimited)
{ 
	_CloseLimited = CloseLimited;
}

void utPathDirector::SetDirMask(long DirMask)
{ 
	_DirMask = (short)DirMask;
}