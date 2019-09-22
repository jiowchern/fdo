#include "stdafx.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#include "JLPathFinder.h"

//---------------------------------------------------------------------------
namespace FDO
{
//---------------------------------------------------------------------------
JLPathFinder _JLPathFinder;     // 宣告實體物件
//---------------------------------------------------------------------------

JLPathFinder::JLPathFinder()
{
    m_iMapWidth     = 0;
    m_iMapHeight    = 0;  
    m_pMapData      = NULL;
}
//---------------------------------------------------------------------------
JLPathFinder::~JLPathFinder()
{
    // there are nothing to do...
}
//---------------------------------------------------------------------------
// Reset the path values
void JLPathFinder::vResetPath( void )
{
	// Reset the nodes
	for( int iLoop = 0; iLoop < PATHFINDER_MAX_NODES; ++iLoop )
    {
		m_OpenList[iLoop].vReset();
		m_ClosedList[iLoop].vReset();
	}

	// Reset state
	m_iStartX = 0;
	m_iStartY = 0;
	m_iEndX = 0;
	m_iEndY = 0;
	m_bGoalFound = false;
	m_iActiveOpenNodes = 0;
	m_iActiveClosedNodes = 0;

	// Set completed path #nodes to 0
	m_CompletePath.m_iNumNodes = 0;
}
//---------------------------------------------------------------------------
// 設定碰撞層資料
//---------------------------------------------------------------------------
void JLPathFinder::vSetCollisionData( int iX, int iY, unsigned char* pData )
{
    m_iMapWidth  = iX;
    m_iMapHeight = iY;
    m_pMapData   = pData;
}
//---------------------------------------------------------------------------
// 判斷是否可行走 1:碰撞層  others:可行走區
//---------------------------------------------------------------------------
int JLPathFinder::iGetMapCost(int iX, int iY)
{
    if( m_pMapData == NULL )
        return -1;

    // If out of horizontal bounds return impassable
	if( iX < 0 || iX >= m_iMapWidth )
		return -1;

	// If out of vertical bounds return impassable
	if( iY < 0 || iY >= m_iMapHeight )
		return -1;

	// Return impassable 
	if( m_pMapData[ iX+(iY*m_iMapWidth) ] == 1 )
		return -1;
	else
		return 2;
}
//---------------------------------------------------------------------------
// Set up the start and end positions.  This is required
// to prep the path for pathfinding.
//---------------------------------------------------------------------------
void JLPathFinder::vSetStartState( int iStartX, int iStartY, int iEndX, int iEndY )
{
	// Reset the path in-case one already in memory
	vResetPath();
	
	// Setup the start state
	m_iStartX = iStartX;
	m_iStartY = iStartY;
	m_iEndX = iEndX;
	m_iEndY = iEndY;  
}
//---------------------------------------------------------------------------
// Get the distance from the node to the goal
//---------------------------------------------------------------------------
int JLPathFinder::iGetGoalDistCost( int iX, int iY, int iCost )
{
	int iGoal = abs(m_iEndX - iX) + abs(m_iEndY - iY);

	iGoal *= iCost;

	return iGoal;
}
//---------------------------------------------------------------------------
// Get the distance from the node to the start position
//---------------------------------------------------------------------------
int JLPathFinder::iGetStartDistCost( int iParentStartCost, int iCost )
{
    int iGoal = iParentStartCost + iCost;

	return iGoal;
}
//---------------------------------------------------------------------------
// Check to see if the node exists on the open
// path list.
//---------------------------------------------------------------------------
bool JLPathFinder::bDoesNotExistOnOpenList( int iX, int iY )
{
	for( int iLoop = 0; iLoop < PATHFINDER_MAX_NODES; ++iLoop )
    {
		if( m_OpenList[iLoop].m_bEnumerated )
        {
			if( (m_OpenList[iLoop].m_iX == iX) && (m_OpenList[iLoop].m_iY == iY) )
				return true;
		}
        else
        {
            break;
        }
	}

	return false;
}
//---------------------------------------------------------------------------
// Check to see if the node exists on the closed
// path list.
//---------------------------------------------------------------------------
bool JLPathFinder::bDoesNotExistOnClosedList( int iX, int iY )
{
	for( int iLoop = 0; iLoop < PATHFINDER_MAX_NODES; ++iLoop )
    {
		if( m_ClosedList[iLoop].m_bEnumerated )
        {
			if( (m_ClosedList[iLoop].m_iX == iX) && (m_ClosedList[iLoop].m_iY == iY) )
				return true;
		}
        else
        {
           // break;
        }
	}

	return false;
}
//---------------------------------------------------------------------------
// Create a new node on the closed or open list or
// update a node on the open list
// 0 = Open List
// 1 = Closed List
//---------------------------------------------------------------------------
int JLPathFinder::iAddNodeToList( int iList, int iX, int iY, CPathNode *ParentNode, int iCost )
{
	// Add to the open list or update the open list
	if( iList == 0 )
    {
		// Node not blocked
		// Check to see if already on the closed list
		bool bExistsOnClosed = bDoesNotExistOnClosedList( iX, iY );
		
		// Node does not exist
		// Check to see if already on the closed list
		if( !bExistsOnClosed )
        {
			// Node not blocked
			// Check to see if already on the open list
			bool bExistsOnOpen = bDoesNotExistOnOpenList( iX, iY );
			// Node does not exist
			// Add it to the open list
			if( !bExistsOnOpen )
            {
				// Find a non-enumerated node on the open list
				for( int iLoop = 0; iLoop < PATHFINDER_MAX_NODES; ++iLoop )
                {
					// Check enumeration
					if( !m_OpenList[iLoop].m_bEnumerated )
                    {
						// Set it to active
						m_OpenList[ iLoop ].m_bEnumerated = true;
						// Set its position on the map
						m_OpenList[ iLoop ].m_iX = iX;
						m_OpenList[ iLoop ].m_iY = iY;
						// Set its distance to the goal cost
						m_OpenList[ iLoop ].m_iDistFromGoalCost = iGetGoalDistCost( iX, iY, iCost );
						// Set its distance from the start cost
						m_OpenList[ iLoop ].m_iDistFromStartCost = iGetStartDistCost( ParentNode->m_iDistFromStartCost, iCost );
						// Set its total cost
						m_OpenList[ iLoop ].m_iTotalCost = m_OpenList[ iLoop ].m_iDistFromGoalCost + m_OpenList[ iLoop ].m_iDistFromStartCost;
						// Set the parent node
						m_OpenList[ iLoop ].m_Parent = ParentNode;  

						// Check if goal is found
						if( (iX == m_iEndX) && (iY == m_iEndY) )
                        {
							m_bGoalFound = true;
							m_iGoalNode = iLoop;
						}

						// Tally open nodes
						++m_iActiveOpenNodes;

						// Done, break out
						return( iLoop );
					}
				}
				// Could not find free node, exit out
				return( -1 );
			}
			// Node already exists on the open list
			// Check to see if the cost from this parent would be cheaper
			// than what is set for the node already
			else
            {
				// Find the node
				for( int iLoop = 0; iLoop < PATHFINDER_MAX_NODES; ++iLoop )
                {
					if( m_OpenList[iLoop].m_bEnumerated )
                    {
						if( (m_OpenList[iLoop].m_iX == iX) && (m_OpenList[iLoop].m_iY == iY) )
						{
							// Figure out distance from start cost for
							// the new node
							int iStartCost = iGetStartDistCost( ParentNode->m_iDistFromStartCost, iCost );

							// Check if the "old" node's distance from start cost is greater
							// than the new node.  If so, set the old node's parent to
							// be the parent node.

							if( m_OpenList[ iLoop ].m_iDistFromStartCost > iStartCost )
                            {
								// Set the parent
								m_OpenList[ iLoop ].m_Parent = ParentNode;
								// Recalculate start cost
								m_OpenList[ iLoop ].m_iDistFromStartCost = iGetStartDistCost( iStartCost, iCost );
								// Recalculate total cost
								m_OpenList[ iLoop ].m_iTotalCost = m_OpenList[ iLoop ].m_iDistFromGoalCost + m_OpenList[ iLoop ].m_iDistFromStartCost;
							}

							// Done, break out
							return( iLoop );
						}
					}
                    else
                    {
                        break;
                    }
				}
			}
		}
		// Node exists on closed list already
		// so ignore it
		else
        {
			return( 0 );
		}
	}
	// Transfer the node to the closed list
	else
    {
		// Remove it from the open list
		ParentNode->m_bEnumerated = false;
		// Find a non-enumerated node on the closed list
		for( int iLoop = 0; iLoop < PATHFINDER_MAX_NODES; ++iLoop )
        {
			// Check enumeration
			if( !m_ClosedList[iLoop].m_bEnumerated )
            {
				// Set it to active
				m_ClosedList[ iLoop ].m_bEnumerated = true;
				// Copy the attributes over
				m_ClosedList[ iLoop ].m_iDistFromGoalCost = ParentNode->m_iDistFromGoalCost;
				m_ClosedList[ iLoop ].m_iDistFromStartCost = ParentNode->m_iDistFromStartCost;
				m_ClosedList[ iLoop ].m_iTotalCost = m_ClosedList[ iLoop ].m_iDistFromGoalCost + m_ClosedList[ iLoop ].m_iDistFromStartCost;
				m_ClosedList[ iLoop ].m_iX = ParentNode->m_iX;
				m_ClosedList[ iLoop ].m_iY = ParentNode->m_iY;
				m_ClosedList[ iLoop ].m_Parent = ParentNode->m_Parent;
				
				// Tally open and closed nodes
				--m_iActiveOpenNodes;
				++m_iActiveClosedNodes;

				// Done, break out
				return( iLoop );
			}
		}
		// Could not find free node, exit out
		return( -1 );
	}

	// Invalid request type, error out
	return( -1 );
}
//---------------------------------------------------------------------------
// Find all open nodes around the given node and add them
// to the open node list.
//---------------------------------------------------------------------------
bool JLPathFinder::bOpenNodes( CPathNode *node )
{
	int		iCost, iRet;
	bool	bFoundOpenNode = false;

	// Check UP
    iCost = iGetMapCost( node->m_iX, node->m_iY-1 );
	if( iCost != -1 )
    {
		// Add the node to the open list
		iRet = iAddNodeToList( 0, node->m_iX, node->m_iY-1, node, iCost );
		// Check for failure, return out if fail
		if( iRet == -1 )  
			return false;

		bFoundOpenNode = true;
	}

	// Check RIGHT
    iCost = iGetMapCost( node->m_iX+1, node->m_iY );
	if( iCost != -1 )
    {
		// Add the node to the open list
		iRet = iAddNodeToList( 0, node->m_iX+1, node->m_iY, node, iCost );
		// Check for failure, return out if fail
		if( iRet == -1 )
			return false;

		bFoundOpenNode = true;
	}

	// Check DOWN
    iCost = iGetMapCost( node->m_iX, node->m_iY+1 );
	if( iCost != -1 )
    {
		// Add the node to the open list
		iRet = iAddNodeToList( 0, node->m_iX, node->m_iY+1, node, iCost );
		// Check for failure, return out if fail
		if( iRet == -1 )
			return false;

		bFoundOpenNode = true;
	}

	// Check LEFT
    iCost = iGetMapCost( node->m_iX-1, node->m_iY );
	if( iCost != -1 )
    {
		// Add the node to the open list
		iRet = iAddNodeToList( 0, node->m_iX-1, node->m_iY, node, iCost );
		// Check for failure, return out if fail
		if( iRet == -1 )
			return false;

		bFoundOpenNode = true;
	}

	// DIAGONALS

	// Check UP-RIGHT
    iCost = iGetMapCost( node->m_iX+1, node->m_iY-1 );
	if( iCost != -1 )
    {
		// Add the node to the open list
		iRet = iAddNodeToList( 0, node->m_iX+1, node->m_iY-1, node, iCost );
		// Check for failure, return out if fail
		if( iRet == -1 )
			return false;

		bFoundOpenNode = true;
	}

	// Check DOWN-RIGHT
    iCost = iGetMapCost( node->m_iX+1, node->m_iY+1 );
	if( iCost != -1 )
    {
		// Add the node to the open list
		iRet = iAddNodeToList( 0, node->m_iX+1, node->m_iY+1, node, iCost );
		// Check for failure, return out if fail
		if( iRet == -1 )
			return false;

		bFoundOpenNode = true;
	}

	// Check DOWN-LEFT
    iCost = iGetMapCost( node->m_iX-1, node->m_iY+1 );
	if( iCost != -1 )
    {
		// Add the node to the open list
		iRet = iAddNodeToList( 0, node->m_iX-1, node->m_iY+1, node, iCost );
		// Check for failure, return out if fail
		if( iRet == -1 )
			return false;

		bFoundOpenNode = true;
	}

	// Check UP-LEFT
    iCost = iGetMapCost( node->m_iX-1, node->m_iY-1 );
	if( iCost != -1 )
    {
		// Add the node to the open list
		iRet = iAddNodeToList( 0, node->m_iX-1, node->m_iY-1, node, iCost );
		// Check for failure, return out if fail
		if( iRet == -1 )
			return false;
		
		bFoundOpenNode = true;
	}

	return bFoundOpenNode;
}
//---------------------------------------------------------------------------
// Look through the path and find the cheapest node in the closed list
//---------------------------------------------------------------------------
int JLPathFinder::iFindCheapestNode( void )
{
	int iCheapestCost = 9999999;
	int iCheapestNode = -1;
	int iNewNode = -1;

	// Loop through every node in the open list
	for( int iLoop = 0; iLoop < PATHFINDER_MAX_NODES; ++iLoop )
    {
		// Check node's cost if it exists
		if( m_OpenList[iLoop].m_bEnumerated )
        {
			// Check if node cost is less than current lowest
			if( m_OpenList[iLoop].m_iTotalCost < iCheapestCost )
            {
				iCheapestNode = iLoop;
				iCheapestCost = m_OpenList[iLoop].m_iTotalCost;
			}
		}
        else
        {
            break;
        }
	}

	// Check if a cheapest node was found
	if( iCheapestNode != -1 )
    {
		// Transfer the cheapest node to the closed list
		iNewNode = iAddNodeToList( 1, 0, 0, &m_OpenList[ iCheapestNode ] );
	}

	return iNewNode;
}
//---------------------------------------------------------------------------
// Calculate the best path from point A to point B
//---------------------------------------------------------------------------
bool JLPathFinder::bFindPath( int iMaxNodes, const int& iIgnoreStep )
{
    if( iMaxNodes > PATHFINDER_MAX_NODES )
        iMaxNodes = PATHFINDER_MAX_NODES;

	CPathNode* pGoalNode = NULL;
	int	iCurNode = 0;
	
	// Put start node in closed list
	m_ClosedList[ 0 ].m_bEnumerated = true;
	// Set position
	m_ClosedList[ 0 ].m_iX = m_iStartX;
	m_ClosedList[ 0 ].m_iY = m_iStartY;
	// Set the Goal
	m_ClosedList[ 0 ].m_iDistFromGoalCost = iGetGoalDistCost( m_iStartX, m_iStartY, 0 );
	// Set the total cost of this node
	m_ClosedList[ 0 ].m_iTotalCost = m_ClosedList[ 0 ].m_iDistFromGoalCost;

	// Find the path
	while( 1 )
    {    
		// Open nodes around the start node
		bOpenNodes( &m_ClosedList[ iCurNode ] );

		// Check if goal state found
		if( m_bGoalFound )
        {
			// Store the completed path
			int iTotalNodes = 0;

			// Count total # of nodes
			pGoalNode = &m_OpenList[m_iGoalNode];

			while( pGoalNode->m_Parent != NULL )
            {
				pGoalNode = pGoalNode->m_Parent;
				++iTotalNodes;
			};
			// Account for the last node
			++iTotalNodes;

			// Store # of nodes
			m_CompletePath.m_iNumNodes = iTotalNodes;

			// Invert the path and store it
			pGoalNode = &m_OpenList[m_iGoalNode];

            // 紀錄完整路徑
			while( iTotalNodes > 0 )
            {
				--iTotalNodes;
				m_CompletePath.m_Path[iTotalNodes] = pGoalNode;
				pGoalNode = pGoalNode->m_Parent;
			};

            // 忽略節點路徑
            if( iIgnoreStep > 0 )
            {
                m_CompletePath.m_iNumNodes -= iIgnoreStep;
                if( m_CompletePath.m_iNumNodes < 1 )
                    m_CompletePath.m_iNumNodes = 1;
            }

            // 紀錄轉折點路徑
            int iDiffX0 = 0, iDiffX1 = 0, iDiffY0 = 0, iDiffY1 = 0;

            m_CompletePath.m_iNumTurnNodes = 0;

            for( int i=1; i<m_CompletePath.m_iNumNodes; ++i )
            {
                iDiffX1 = m_CompletePath.m_Path[i]->m_iX - m_CompletePath.m_Path[i-1]->m_iX;
                iDiffY1 = m_CompletePath.m_Path[i]->m_iY - m_CompletePath.m_Path[i-1]->m_iY;

                if( ( iDiffX1 != iDiffX0 ) || ( iDiffY1 != iDiffY0 ) )
                {
                    m_CompletePath.m_TurnPath[m_CompletePath.m_iNumTurnNodes++] = m_CompletePath.m_Path[i-1];

                    iDiffX0 = iDiffX1;
                    iDiffY0 = iDiffY1;
                }
            }

            // 紀錄終點
            m_CompletePath.m_TurnPath[m_CompletePath.m_iNumTurnNodes++] = m_CompletePath.m_Path[m_CompletePath.m_iNumNodes-1];

			return true;
		}
		// Find cheapest open node and add it to the
		// closed list
		iCurNode = iFindCheapestNode();

		// Check to see if no solution is possible
		if( iCurNode == -1 )
        {
			return false;
		}

		// Break out if max # of closed nodes reached
		if( m_iActiveClosedNodes >= iMaxNodes )
			return false;
	}   
}
//---------------------------------------------------------------------------
PathFinder::PathFinder( int iMaxPointCount ) : m_iMaxPointCount( iMaxPointCount ) /*, m_pFindPathCore( 0 )*/ , m_bFindPath( false ) , 
	m_iStartX( 0 ) , m_iStartY( 0 ) , m_iGoalX( 0 ) , m_iGoalY( 0 ) , m_iIgnoreStep( 0 ) , m_bInitSuccess( false ) 
{
}
//---------------------------------------------------------------------------
PathFinder::~PathFinder()
{
	//delete m_pFindPathCore ;
}
//---------------------------------------------------------------------------
void PathFinder::InitPathFind( int iMapWidth , int iMapHeight , unsigned char* pCollisionLayerData )
{
	//m_pFindPathCore = new utPathDirector ;
	int iMaxCount = ( iMapWidth > iMapHeight ) ? iMapWidth : iMapHeight ;
	m_FindPathCore.Destroy() ;
	m_bInitSuccess = false ;
	//int iRet = m_pFindPathCore->Initialize( iMapWidth, iMapHeight , iMaxCount ) ;
	int iRet = m_FindPathCore.Initialize( iMapWidth, iMapHeight , iMaxCount ) ;
	if( iRet == 0 )
	{
		//m_pFindPathCore->_SceneWidth = iMapWidth ;
		//m_pFindPathCore->_SceneHeight = iMapHeight ;
		//m_pFindPathCore->_arrCollide = pCollisionLayerData ;
		//m_pFindPathCore->SetOpenLimited( iMapWidth * iMapHeight ) ;
		//m_pFindPathCore->SetCloseLimited( iMapWidth* iMapHeight ) ;
		//m_pFindPathCore->SetDirMask( 0xff ) ;
		m_bInitSuccess = true ;
		m_FindPathCore._SceneWidth = iMapWidth ;
		m_FindPathCore._SceneHeight = iMapHeight ;
		m_FindPathCore._arrCollide = pCollisionLayerData ;
		m_FindPathCore.SetOpenLimited( iMapWidth * iMapHeight ) ;
		m_FindPathCore.SetCloseLimited( iMapWidth* iMapHeight ) ;
		m_FindPathCore.SetDirMask( 0xff ) ;
	}
	
	//else SAFE_DELETE( m_pFindPathCore ) ;
}
//---------------------------------------------------------------------------
bool PathFinder::bFindPath( int iStartX , int iStartY , int iGoalX , int iGoalY , int iIgnoreStep /*= 0*/ )
{
	if(iStartX >= 0 && iStartX <= m_FindPathCore._SceneWidth && iStartY >=0 && iStartY <= m_FindPathCore._SceneHeight)
	{

		m_iStartX = iStartX ;
		m_iStartY = iStartY ;
		m_iGoalX = iGoalX ;
		m_iGoalY = iGoalY ;
		m_iIgnoreStep = iIgnoreStep ;

		if( m_bInitSuccess && m_FindPathCore.FindPath( iStartX, iStartY, iGoalX, iGoalY ) == 1 )
		{
			m_bFindPath = true ;	
		}
		else
		{
			std::string out;
			StringFormat8(out,"\n自動尋徑失敗[Core] %d,%d -> %d,%d",iStartX, iStartY, iGoalX, iGoalY);
			OutputDebugStringA(out.c_str());
			m_bFindPath = false ;
		}

#pragma pack( push )
#pragma pack( 1 )
		struct tmpPos
		{
			sint16 x;
			sint16 y;
		};
#pragma pack( pop )

		sint nCollisionSize		= m_FindPathCore._SceneWidth * m_FindPathCore._SceneHeight;
		tmpPos* poss			= new tmpPos[nCollisionSize];
		sint	possCount		= nCollisionSize ;			
		tmpPos* simpposs		= new tmpPos[nCollisionSize];
		sint	simppossCount	= nCollisionSize ;
		char*	Direction		= new char[ nCollisionSize ] ;
		sint	nDirection		= nCollisionSize;

		m_FindPathCore.GetPosPath((sint16*)poss,&possCount ,(sint16*)simpposs,&simppossCount,m_iIgnoreStep);
		m_FindPathCore.GetDirPath( Direction , nDirection ) ;

// 		std::string out;	
// 		StringFormat8(out,"\n自動尋徑 : 點(start)");
// 		OutputDebugStringA(out.c_str());
 		m_PointList.clear() ; 
 		for (sint i = 0 ; i < possCount ; ++i)
 		{
 			m_PointList.push_back(sPos(poss[i].x , poss[i].y, Direction[i]));
// 			StringFormat8(out,"[%d,%d](%d) ",poss[i].x , poss[i].y , Direction[i]);
// 			OutputDebugStringA(out.c_str());
 		}

		
// 		StringFormat8(out,"\n自動尋徑 : 轉彎點(start)");
// 		OutputDebugStringA(out.c_str());
 		m_NodeList.clear() ; 
// 
 		uint nTmpDir = -1;
 		for (sint i = 0 ; i < possCount ; ++i)
 		{
 			if(nTmpDir != Direction[i])
 			{
 				m_NodeList.push_back(sPos(poss[i].x , poss[i].y, Direction[i]));
 				nTmpDir = Direction[i];
// 				StringFormat8(out,"[%d,%d](%d) ",poss[i].x , poss[i].y , Direction[i]);
// 				OutputDebugStringA(out.c_str());
 			}
 		}
		
// 		StringFormat8(out,"\n自動尋徑 : 轉彎點對照");
// 		OutputDebugStringA(out.c_str());
// 		for (sint i = 0 ; i < simppossCount ; ++i)
// 		{						
// 			StringFormat8(out,"[%d,%d] ",simpposs[i].x , simpposs[i].y);
// 			OutputDebugStringA(out.c_str());
// 		}

		delete [] poss;
		delete [] simpposs;			
		delete [] Direction;

		//FindResult() ;
		return m_bFindPath ;
	}
	return false;
}
//---------------------------------------------------------------------------		

void PathFinder::FindResult()
{
	m_PointList.clear() ; 
	//m_NodeList.clear() ;
	int iMaxPointArrayCount = ( m_iMaxPointCount + 1 ) * 2 ;
	int iMaxPathCount = m_iMaxPointCount + 1 ;
	short *Point = new short[ iMaxPointArrayCount ] ;					// 多兩格填-1﹝結尾值﹞
	short *Path = new short[ m_iMaxPointCount ] ;						// 多兩格填-1﹝結尾值﹞
	char  *Direction = new char[ iMaxPathCount ] ;						// 多兩格填-1﹝結尾值﹞

	int iPointCount = m_iMaxPointCount ;
	int	iPathCount = iMaxPathCount ;

	//m_pFindPathCore->GetPosPath( Point , &iPointCount, Path , &iPathCount , m_iIgnoreStep ) ;
	m_FindPathCore.GetPosPath( Point , &iPointCount, Path , &iPathCount , m_iIgnoreStep ) ;

	if( iPointCount > 1  )
	{
		m_FindPathCore.GetDirPath( Direction , iPointCount ) ;
		//m_pFindPathCore->GetDirPath( Direction , iPointCount ) ;

		for( int i = 0 , j = 0 ; i < iPointCount * 2 ; i += 2 , ++j )
		{
			m_PointList.push_back( sPos( Point[ i ] , Point[ i + 1 ] ,  *( Direction + j ) ) ) ;
		}
	}

	vector< short > tmpPath( Path , Path + iPathCount * 2 ) ;
	for( int i = 0 , j = 0 ; i < iPathCount ; ++i , j += 2 )
	{
		m_NodeList.push_back( sPos( tmpPath[ j ] , tmpPath[ j + 1 ] ) ) ;
	}
	
	delete [] Point ;
	delete [] Path ;
	delete [] Direction ;
}
//---------------------------------------------------------------------------
void PathFinder::GetPointList( PointList& listResult )
{
	if( !m_PointList.empty() )
	{
		listResult.clear() ;
		listResult.assign( m_PointList.begin() , m_PointList.end() ) ;
	}
}
//---------------------------------------------------------------------------
void PathFinder::GetNodeList( NodeList& listResult )
{
	if( !m_NodeList.empty() )
	{
		listResult.clear() ;
		listResult.assign( m_NodeList.begin() , m_NodeList.end() ) ;
	}
}
//---------------------------------------------------------------------------
RolePathFinder::RolePathFinder() : PathFinder()
{
}
//---------------------------------------------------------------------------
RolePathFinder::~RolePathFinder()
{
}
//---------------------------------------------------------------------------
bool RolePathFinder::bFindPath(  int iRoleKind , int iStartX , int iStartY , int iGoalX , int iGoalY , int iIgnoreStep /*=0*/ )
{
	
	if( iRoleKind < 0 || iRoleKind >= PathFinder_RoleCount )
		return false ;

	sRolePathFindInfo& RoleInfo = m_RolePathInfo[ iRoleKind ] ;

	RoleInfo.m_iStartX = iStartX ;
	RoleInfo.m_iStartY = iStartY ;
	RoleInfo.m_iGoalX = iGoalX ;
	RoleInfo.m_iGoalY = iGoalY ;
	RoleInfo.m_iIgnoreStep = iIgnoreStep ;

	RoleInfo.m_pPointList->clear() ;
	RoleInfo.m_pNodeList->clear() ;

	RoleInfo.m_iterPointCur = RoleInfo.m_pPointList->end() ;
	RoleInfo.m_iterNodeCur  = RoleInfo.m_pNodeList->end() ;

// 	if(iStartX == iGoalX && iStartY == iGoalY)
// 		return true;

	if( !RoleInfo.m_TaskID )
	{
		if( iRoleKind == PathFinder_MainRole )
		{
			if( !FDO::TaskManager::GetSingletonPtr()->CreateThread( MainRole_Use_ThreadID ) )
				Assert( 0 , "ThreadId 重覆定義" ) ;
			
			RoleInfo.m_TaskID = FDO::TaskManager::GetSingletonPtr()->AddTask( MainRole_Use_ThreadID , this , &RolePathFinder::ThreadRun, (void*)iRoleKind ) ;
			FDO::TaskListener::GetSingletonPtr()->SetRequest( RoleInfo.m_TaskID , this ) ;
			DoPathFindStart( RoleInfo ) ;
			
		}
		else if( iRoleKind == PathFinder_PetRole )
		{
			if( !FDO::TaskManager::GetSingletonPtr()->CreateThread( PetRole_Use_ThreadID ) )
				Assert( 0 , "ThreadId 重覆定義" ) ;
			
			RoleInfo.m_TaskID = FDO::TaskManager::GetSingletonPtr()->AddTask( PetRole_Use_ThreadID , this , &RolePathFinder::ThreadRun, (void*)iRoleKind ) ;	
			FDO::TaskListener::GetSingletonPtr()->SetRequest( RoleInfo.m_TaskID , this ) ;
			DoPathFindStart( RoleInfo ) ;
			
		}
	}
	return true;
}
//---------------------------------------------------------------------------
int	RolePathFinder::ThreadRun( void* pdata )
{
	int iRoleKind = ( int )pdata ;
	sRolePathFindInfo& RoleInfo = m_RolePathInfo[ iRoleKind ] ;
	bool Success = PathFinder::bFindPath( RoleInfo.m_iStartX , RoleInfo.m_iStartY , RoleInfo.m_iGoalX , RoleInfo.m_iGoalY , RoleInfo.m_iIgnoreStep );
	
	if( m_PointList.size() )
	{		

		GetPointList( *RoleInfo.m_pPointList ) ;
		GetNodeList( *RoleInfo.m_pNodeList ) ;

		RoleInfo.m_iterPointCur = RoleInfo.m_pPointList->begin() ;
		RoleInfo.m_iterNodeCur  = RoleInfo.m_pNodeList->begin() ;

	}
	else
	{
		RoleInfo.m_pPointList->clear();
		RoleInfo.m_pNodeList->clear();

		RoleInfo.m_iterPointCur = RoleInfo.m_pPointList->end() ;
		RoleInfo.m_iterNodeCur  = RoleInfo.m_pNodeList->end() ;

		OutputDebugStringA("\n自動尋徑失敗");
	}
	
	return NSTask::Task::END;
}
//---------------------------------------------------------------------------
void RolePathFinder::TaskFinish( int nThreadId , TaskID nId )
{
	for( int i = 0 ; i < PathFinder_RoleCount ; ++i )
	{
		sRolePathFindInfo& RoleInfo = m_RolePathInfo[ i ] ;
		if( RoleInfo.m_TaskID == nId )
		{
			FDO::TaskManager::GetSingletonPtr()->DestoryThread( nThreadId ) ;
			DoPathFinished( RoleInfo ) ;
			RoleInfo.m_TaskID = 0 ;
			break ;
		}
	}	
}
//---------------------------------------------------------------------------
void RolePathFinder::DoPathFindStart( sRolePathFindInfo& Info )
{
}
//---------------------------------------------------------------------------
void RolePathFinder::DoPathFinished( sRolePathFindInfo& Info )
{
}
//---------------------------------------------------------------------------
} //namespace FDO
//---------------------------------------------------------------------------
