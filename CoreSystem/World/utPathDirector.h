#pragma once

#include <windows.h>
#include <stdio.h>
//#include "utBase.h"
//#include "utTree.h"

typedef unsigned long ADWORD;

#define	OBLIQUE_ONE	1						// 定義斜角是否以一格計算；0：斜角以真實距離﹝√2格﹞ 1：斜角以一格距離計算

#define COST_MAX 30000
#define tile_pos(x,y) (((ADWORD)y<<16)|x)	// 將X,Y座標合存成一個值，Y座標存放高兩Bytes，X座標存放低兩Bytes
#define tile_x(pos)   (pos&0xffff)			// 將X座標自pos中提取出來，直接取低兩Bytes
#define tile_y(pos)   (pos>>16)				// 將Y座標自pos中提取出來

//const WORD MAX_PATH_FINDER_POINT_COUNT = 10;

//typedef char (utBase::*CharSSFunc)(short x,short y);
//typedef short (utBase::*ShortSSFunc)(short x,short y, short endx, short endy);

class utPathDirector//: public utBase
{
private:
    static short incx[8];
    static short incy[8];
    static char inc2r[9];

    struct TAstarNode
    {
        ADWORD Pos;         // 該點的座標(16,16)的模式保存(y,x)
        short ActualCost;   // 保存從起點到該節點的實際開銷
        short EstimateCost; // 保存此點的估價開銷
        TAstarNode *Father; // 此點的父節點
        TAstarNode *Next;   // 在Open或者Next鏈表中的下一個節點
        char Modified;      // 該節點是否被修改過，記錄而備清除 byte1:空 byte2: Open, byte4: Close
		BYTE Dir;			// 由父節點至此節點的方向代碼
    };
	
	//utRBTreeS<short, TAstarNode *> _treOpen;
	short **_arrSimpPos;

    TAstarNode **Node;      // 對應地圖中每個節點
    TAstarNode *Open;       // 保存沒有處理的按估計值排序的節點
    TAstarNode *Close;      // 保存處理過的節點
    TAstarNode **Modify;    // 保存修改過的節點
    int height;             // 地圖的高度
    int width;              // 地圖的寬度
    ADWORD MaxSize;         // 最大面積即height*width
    ADWORD ModifyPoint;     // Modify陣列的指標
    ADWORD OpenCount;       // Open佇列中的節點數目
    ADWORD CloseCount;      // Close佇列裏面的節點數目
    ADWORD _OpenLimited;     // Open佇列最多節點數目
    ADWORD _CloseLimited;    // Close佇列最多節點數目
    short _DirMask;          // 搜索方向的位元遮罩，0-7位元為從上開始順時針八個方向
    ADWORD MinPos;          // 終點或最接近終點的節點
	//CharSSFunc Moveable;	// 檢查地圖是否可以移動函數指標
	//ShortSSFunc JudgeCost;  // 取得估計值的函數指標
	static const BYTE m_iSeqNoTH;             // threshold for comparing seq. no.(0x3FFFFFFF)
	static const BYTE m_iMaxSeqNo;            // maximum sequence number used in UDT(0x7FFFFFFF)

	
	char AddToOpenQueue(TAstarNode *node);  // 將節點排序加入Open佇列
    char GetFromOpenQueue();                // 從Open佇列取出最小的並放入Close佇列
    char PopCloseStack();                   // 取出Close佇列中的節點
    void ClearModified();                   // 清除所有搜索記錄
    char TryTile(short x, short y, short endx, short endy, TAstarNode *Father, BYTE Dir);
public:
	utPathDirector(void);
	virtual ~utPathDirector(void);
	
	WORD _SceneWidth;
	WORD _SceneHeight;
	BYTE *_arrCollide;

	inline static const char DirCompare(const char& Dir1, const char& Dir2)
	{
		return (abs(Dir1 - Dir2) < m_iSeqNoTH) ? (Dir1 - Dir2) : (Dir2 - Dir1);
	}

	inline static const char DirOffset(const char& Dir1, const char& Dir2)
	{
		if (abs(Dir1 - Dir2) < m_iSeqNoTH)
			return Dir2 - Dir1;

		if (Dir1 < Dir2)
			return Dir2 - Dir1 - m_iMaxSeqNo;

		return Dir2 - Dir1 + m_iMaxSeqNo;		// if (Dir1 >= Dir2)
	}

	// 測試用
	short Judge(short x,short y, short endx, short endy, BYTE PreDir, BYTE Dir)
	{
#if (OBLIQUE_ONE == 1)
		//// 使用曼哈頓方法（Manhattan method）搭配"慣性加權"為估算公式
		//return (short)(m_iSeqNoTH * (abs(x - endx) + abs(y - endy)) + ((DirCompare(Dir, PreDir) > 0) ? DirOffset(PreDir, Dir) : DirOffset(Dir, PreDir)));
		// 使用曼哈頓方法（Manhattan method）搭配"慣性加權"為估算公式
		return (short)(abs(x - endx) + abs(y - endy));
#else
		// 使用曼哈頓方法（Manhattan method）為估算公式
		return (short)(10 * (abs(x - endx) + abs(y - endy)));
#endif //(OBLIQUE_ONE == 1)
	}
	char mPossible(short x,short y) 
	{ 
		//if (x < 0 || y < 0 || x >= (short)_SceneWidth || y >= (short)_SceneHeight) return (char)0;
		//return (char)(((*(_arrCollide + y * _SceneWidth + x)) == 1) ? 0 : 1);
		return ((x >= 0) && (y >= 0) && (x < (short)_SceneWidth) && (y < (short)_SceneHeight)) ? (((*(_arrCollide + y * _SceneWidth + x)) == 1) ? 0 : 1) : 0;
	}
	// 初始化路徑搜尋物件；mapw: 場景寬, maph: 場景高, MaxDistance: 最大搜尋距離
	int Initialize(int MapWidth, int MapHeight, WORD MaxDistance);
    int Destroy();
	// 路徑尋找主函數；回傳值：-2: Close Queue已空 -1: Open Queue已空 1: 可到達終點 2: 路徑長度超過設定之最小值
    int FindPath(short startx, short starty, short endx, short endy);
    int GetPosPath(short *PosXY, int *MaxPosCount, short *SimpPosXY, int *SimpMaxPosCount, short KeepBackDistance = 0);
	// 取得每次以八方向位移1格的路徑陣列
    int GetDirPath(char *PosDir, int MaxPosCount);

    //void SetJudgeFun(ShortSSFunc JudgeFun);
    //void SetMapCheck(CharSSFunc MapCheck);

	// 設定Open佇列節點數最大值
    void SetOpenLimited(long OpenLimited);
	// 設定Close佇列節點數最大值
    void SetCloseLimited(long CloseLimited);
	// 設定搜索方向的位元遮罩
    void SetDirMask(long DirMask);

};
