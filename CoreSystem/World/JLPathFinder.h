//---------------------------------------------------------------------------
#ifndef _JLPATHFINDER_H_
#define _JLPATHFINDER_H_

#include <math.h>
#include "singleton.h"
#include "utPathDirector.h"
#include "fdo_thread_utility.h"
#define g_PathFinder JLPathFinder::GetSingleton()

const int PATHFINDER_MAX_NODES = 1000;
const int PATHFINDER_MAX_TURN_NODES = 128;
const int PATHFINDER_DEF_COST  = 10;

//---------------------------------------------------------------------------
namespace FDO
{

//---------------------------------------------------------------------------
class CPathNode
{
	private:
		
	public:
		CPathNode *m_Parent;
		int  m_iDistFromStartCost;
		int  m_iDistFromGoalCost;
		int  m_iTotalCost;
		bool m_bEnumerated;
		int  m_iX;
		int  m_iY;

		CPathNode() { vReset(); }

        void vReset()
        {
            m_Parent = NULL;
	        m_iDistFromStartCost = 0;
	        m_iDistFromGoalCost = 0;
	        m_iTotalCost = 0;
	        m_bEnumerated = false;
        }
};
//---------------------------------------------------------------------------
class JLPathFinder : public Singleton<JLPathFinder>
{
	private:

        class CPath
        {
            public:
                CPathNode	*m_Path[ PATHFINDER_MAX_NODES ];
                CPathNode	*m_TurnPath[ PATHFINDER_MAX_TURN_NODES ];
                int			 m_iNumNodes;
                int			 m_iNumTurnNodes;
        };

		int	    m_iStartX;
		int	    m_iStartY;
		int	    m_iEndX;
		int     m_iEndY;
		int     m_iGoalNode;
        bool    m_bGoalFound;
        int	    m_iMapWidth;
        int	    m_iMapHeight;
        unsigned char* m_pMapData;

	public:
        CPathNode    m_OpenList[PATHFINDER_MAX_NODES];
        CPathNode    m_ClosedList[PATHFINDER_MAX_NODES];
		CPath		 m_CompletePath;
		int			 m_iActiveOpenNodes;
		int			 m_iActiveClosedNodes;
		
		JLPathFinder();
		~JLPathFinder();

        void vSetCollisionData( int iX, int iY, unsigned char* pData );
		void vSetStartState( int iStartX, int iStartY, int iEndX, int iEndY );
        void vResetPath();
        bool bFindPath( int iMaxNodes = PATHFINDER_MAX_NODES, const int& iIgnoreStep = 0 );
		bool bOpenNodes( CPathNode *node );
		bool bDoesNotExistOnOpenList( int iX, int iY );
		bool bDoesNotExistOnClosedList( int iX, int iY );
        int  iGetGoalDistCost( int iX, int iY, int iCost = PATHFINDER_DEF_COST );
		int  iGetStartDistCost( int iParentStartCost, int iCost = PATHFINDER_DEF_COST );
		int  iAddNodeToList( int iList, int iX, int iY, CPathNode *ParentNode, int iCost = PATHFINDER_DEF_COST );
		int	 iGetMapCost(int iX, int iY);
		int  iFindCheapestNode();  
};
//---------------------------------------------------------------------------
class PathFinder
{
public :

	enum eDirection
	{
		South ,
		SouthEast ,
		East ,
		NorthEast ,
		North ,
		NorthWest ,
		West ,
		SouthWest ,
		None ,
		DirectionCount ,
	} ;

	struct sPos 
	{
		union
		{
			int m_iPos ;
			struct
			{
				short m_iPosX ;        
				short m_iPosY ;
			} ;
		} ;
		sPos( short iPosX , short iPosY , char iDirection = None ) : m_iPosX( iPosX ) , m_iPosY( iPosY )  {}	
		bool operator == ( const sPos& pos ) const
		{
			return m_iPos == pos.m_iPos ;
		}
		bool operator != ( const sPos& pos )
		{
			return m_iPos != pos.m_iPos ;
		}
		void operator = ( const sPos& pos ) 
		{
			m_iPos = pos.m_iPos ;
		}
	} ;
	
	typedef list< sPos > PointList ;
	typedef list< sPos > NodeList ;

	PathFinder( int iMaxPointCount = 512 ) ;
	virtual ~PathFinder() ;

	void InitPathFind( int iMapWidth , int iMapHeight , unsigned char* pCollisionLayerData ) ;
	virtual bool bFindPath( int iStartX , int iStartY , int iGoalX , int iGoalY , int iIgnoreStep = 0 ) ;
	
	void GetPointList( PointList& listResult ) ;
	void GetNodeList( NodeList& listResult ) ;
	inline bool bFindPath() { return m_bFindPath ; }

protected :
	
	PointList m_PointList ;
	NodeList m_NodeList ;

	int m_iStartX ;
	int m_iStartY ;
	int m_iGoalX ;
	int m_iGoalY ;
	int m_iIgnoreStep ;

	bool m_bFindPath ;
	bool m_bInitSuccess ;
	
private :
	
	int m_iMaxPointCount ; // 最大路徑搜尋陣列元素個數
	utPathDirector m_FindPathCore ;
	void FindResult() ;
} ;
//---------------------------------------------------------------------------
class RolePathFinder : public PathFinder , public TaskInquisitor
{
public :

	enum 
	{
		PathFinder_MainRole ,
		PathFinder_PetRole ,
		PathFinder_RoleCount 
	} ;

	//TreadID
	enum 
	{
		MainRole_Use_ThreadID = 100 ,  
		PetRole_Use_ThreadID ,
	} ;

	typedef PointList::const_iterator CurPointIter ;
	typedef NodeList::const_iterator CurNodeIter ;
	
	struct sRolePathFindInfo		   
	{
		int          m_iRoleKind ;     //角色Type
		int          m_iStartX ;       //起點X
		int          m_iStartY ;       //起點Y
		int          m_iGoalX ;        //終點X
		int          m_iGoalY ;        //終點Y
		int          m_iIgnoreStep ;
		TaskID       m_TaskID ;        //TaskID
		CurPointIter m_iterPointCur ;  //目前走到的座標
		CurNodeIter  m_iterNodeCur ;   //目前走到的節點
		PointList*   m_pPointList ;    //路徑所有點的List
		NodeList*    m_pNodeList ;     //節點list
		
		sRolePathFindInfo()
		{
			m_iRoleKind = 0 ;
			m_iStartX = 0 ;
			m_iStartY = 0 ;
			m_iGoalX = 0 ;
			m_iGoalY = 0 ;
			m_TaskID = 0 ;
			m_iIgnoreStep = 0 ;
			m_pPointList = new PointList ;
			m_pNodeList = new NodeList ;
			m_iterPointCur = m_pPointList->end() ;
			m_iterNodeCur = m_pNodeList->end() ;
		}
		~sRolePathFindInfo()
		{
			delete m_pPointList ;
			delete m_pNodeList ;
		}
	} ;
	
	RolePathFinder() ;
	virtual ~RolePathFinder() ;
	bool bFindPath( int iRoleKind , int iStartX , int iStartY , int iGoalX , int iGoalY , int iIgnoreStep = 0 ) ;

	virtual void DoPathFindStart( sRolePathFindInfo& Info ) ;   //當計算交給Thread時觸發的界面
	virtual void DoPathFinished( sRolePathFindInfo& Info ) ;	//當搜尋到路徑時觸發的界面

protected :

	sRolePathFindInfo m_RolePathInfo[ PathFinder_RoleCount ] ;  //路徑資訊

private :

	int	ThreadRun( void* pdata ) ;								//交給Thread執行的函式
	void TaskFinish( int nThreadId , TaskID nId ) ;             //Thread計算完成
	
} ;
//---------------------------------------------------------------------------
} //namespace FDO
//---------------------------------------------------------------------------
#endif
