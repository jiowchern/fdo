#include "stdafx.h"
#include "RenderComponentTool.h"

void CreateJiugong( RECT& rcBoundingBox , vector< RECT >& Rects , uint uFrameSize )
{
	Rects.resize( Jiugong_Count ) ;

	sint iLeft   = rcBoundingBox.left ;
	sint iTop    = rcBoundingBox.top ;
	sint iRight  = rcBoundingBox.right ;
	sint iBottom = rcBoundingBox.bottom ;
	sint iCenterWidth = RectWidth( rcBoundingBox ) - 2 * uFrameSize ;
	sint iCenterHeight = RectHeight( rcBoundingBox ) - 2 * uFrameSize ;

	//LeftTop
	iRight = iLeft + uFrameSize ;
	iBottom = iTop + uFrameSize ;
	SetRect( &Rects[ Jiugong_LeftTop ] , iLeft , iTop , iRight , iBottom ) ;
	
	//Top
	iLeft = iRight ;
	iRight = iLeft + iCenterWidth ;
	SetRect( &Rects[ Jiugong_Top ] , iLeft , iTop , iRight , iBottom ) ;
	
	//RightTop
	iLeft = iRight ;
	iRight = iLeft + uFrameSize ;
	SetRect( &Rects[ Jiugong_RightTop ] , iLeft , iTop , iRight , iBottom ) ;
	
	//Left
	iLeft = rcBoundingBox.left ;
	iTop = rcBoundingBox.top + uFrameSize ; 
	iRight = iLeft + uFrameSize ;
	iBottom = iTop + iCenterHeight ;
	SetRect( &Rects[ Jiugong_Left ] , iLeft , iTop , iRight , iBottom ) ;

	//Center
	iLeft = iRight ;
	iRight = iLeft + iCenterWidth ;
	SetRect( &Rects[ Jiugong_Center ] , iLeft , iTop , iRight , iBottom ) ;
	
	//Right
	iLeft = iRight ;
	iRight = iLeft + uFrameSize ;
	SetRect( &Rects[ Jiugong_Right ] , iLeft , iTop , iRight , iBottom ) ;
	
	//LeftBottom
	iLeft = rcBoundingBox.left ;
	iTop = iBottom ;
	iRight = rcBoundingBox.left + uFrameSize ;
	iBottom = iTop + uFrameSize ;
	SetRect( &Rects[ Jiugong_LeftBottom ] , iLeft , iTop , iRight , iBottom ) ;
	
	//Bottom
	iLeft = iRight ;
	iRight = iLeft + iCenterWidth ;
	SetRect( &Rects[ Jiugong_Bottom ] , iLeft , iTop , iRight , iBottom ) ;
	
	//RightBottom
	iLeft = iRight ;
	iRight = iLeft + uFrameSize ;
	SetRect( &Rects[ Jiugong_RightBottom ] , iLeft , iTop , iRight , iBottom ) ;

}
