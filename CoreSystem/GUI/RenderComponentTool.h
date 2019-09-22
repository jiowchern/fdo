#ifndef _RenderComponentTool_H_
#define _RenderComponentTool_H_

//九宮Rects的編號
// 0 1 2
// 3 4 5
// 6 7 8
enum eJiugongFramesId
{
	Jiugong_LeftTop ,
	Jiugong_Top ,
	Jiugong_RightTop ,
	Jiugong_Left ,
	Jiugong_Center ,
	Jiugong_Right ,
	Jiugong_LeftBottom ,
	Jiugong_Bottom ,
	Jiugong_RightBottom ,
	Jiugong_Count ,
} ;

//產生九宮的Rects
void CreateJiugong( RECT& rcBoundingBox , vector< RECT >& Rects , uint uFrameSize ) ;




#endif 