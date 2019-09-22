#include "stdafx.h"
#include "mapcontrol.h"
#include "assert.h"
#include "R_BaseType.h"


#define DegreesToRads( degrees ) ((degrees) * (PI/ 180.0f))
#define RadsToDegrees( angle ) ((18.0f) * (angle / PI))

////						靜態變數宣告				//////////

float CTextureCoordControl::s_texcoordcenter = 0.5f ; 
float CTextureCoordControl::s_texcoordrightbottom = s_texcoordcenter * 2.0f; 
float CTextureCoordControl::s_worldviewrect = 150.0f ;
float CTextureCoordControl::s_rotation = 45.0f ; 

////
//		類別：Texture座標控制
////

CTextureCoordControl::CTextureCoordControl( void ) : m_rotated()
{
	Init( ) ;

}

CTextureCoordControl::~CTextureCoordControl( void )
{
	Release( ) ;
}

void CTextureCoordControl::Init( void )
{
	memset( &m_verpos , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_texzoom , 0 , sizeof(D3DXVECTOR2) ) ;

	m_texUVpos.x = 0.5f ;
	m_texUVpos.y = 0.5f ;
	m_cameraUV.x = 0.5f ; 
	m_cameraUV.y = 0.5f ; 

	m_texUVcenter.x = s_texcoordcenter ; 
	m_texUVcenter.y = s_texcoordcenter ;
	m_texUV.x = s_texcoordcenter ;
	m_texUV.y = s_texcoordcenter ;

	m_texmaxzoom = 0.05f ;
	m_texminzoom.x = s_texcoordcenter ;
	m_texminzoom.y = s_texcoordcenter ;

	m_texture = NULL ;
	m_alphatex = NULL ;

	m_texUVleft = 0.0f ;
	m_texUVtop = 0.0f ;
	m_texUVright = s_texcoordrightbottom ;
	m_texUVbottom = s_texcoordrightbottom ;


	SetVertex( &m_mapvertex[0] , 0.0f , 0.0f , 0.0f , 1.0f , m_texUVleft , m_texUVtop  , 0.0f , 0.0f ) ;
	SetVertex( &m_mapvertex[1] , 0.0f , 0.0f , 0.0f , 1.0f , m_texUVleft , m_texUVbottom , 0.0f , 1.0f ) ;
	SetVertex( &m_mapvertex[2] , 0.0f , 0.0f , 0.0f , 1.0f , m_texUVright , m_texUVbottom , 1.0f , 1.0f ) ;
	SetVertex( &m_mapvertex[3] , 0.0f , 0.0f , 0.0f , 1.0f , m_texUVright , m_texUVtop  , 1.0f , 0.0f ) ;

	m_texSize.x = 64.0f ;
	m_texSize.y = 64.0f ;

	//預設值(64,64)
	SetVertexSize( (int)m_texSize.x , (int)m_texSize.y ) ;

	//add 08/1/21
	m_bshare = false ;


	


}

void CTextureCoordControl::Release( void )
{
	if ( m_bshare )
	{
		m_texture = NULL ;
		m_alphatex = NULL ;
	}
	else
	{
		SAFE_RELEASE(m_texture) ;
		SAFE_RELEASE(m_alphatex) ;
	}

}

//說明：取得現有UV座標
void CTextureCoordControl::GetUVpos( float &u , float &v ) const 
{
	u = m_texUVpos.x ;
	v = m_texUVpos.y ; 
}

//說明：取得現有UV四點座標
void CTextureCoordControl::GetUVposRect( float &left , float &top ,
										float &right , float &bottom ) const 
{
	left = m_texUVleft ;
	top = m_texUVtop ; 
	right = m_texUVright ;
	bottom = m_texUVbottom ;
}

//說明：取得縮放係數
void CTextureCoordControl::GetZoom( float &scalex , float &scaley ) const 
{
	scalex = m_texzoom.x  ;
	scaley = m_texzoom.y ;
}

//說明：取得此Texture的長跟寬
void CTextureCoordControl::GetTextureSize(float &texwidth, float &texheight) const
{
	texwidth = m_texSize.x ; 
	texheight = m_texSize.y ;
}

//說明：直接指向Texture資源指標			add 08/1/21
void CTextureCoordControl::SetTexture(LPDIRECT3DTEXTURE9 tex)
{
	assert( tex != NULL ) ;

	//設定分享
	m_bshare = true ;

	m_texture = tex ;

	//更新Texture
	UpdateTextureAtt( ) ;


}

//說明：直接指向AlphaTexture資源指標		add 08/1/21
void CTextureCoordControl::SetAlphaTexture(LPDIRECT3DTEXTURE9 tex)
{
	assert( tex != NULL ) ;
	
	//設定分享
	m_bshare = true ;

	m_alphatex = tex ;
}

//說明：讀取圖片form disk
void CTextureCoordControl::LoadTexture(LPCWSTR filename)
{
	m_bshare = false ;

	D3DXCreateTextureFromFile( g_pD3DDevice , filename , &m_texture ) ;

	//更新Texture
	UpdateTextureAtt( ) ;
}

//說明：讀取alpha圖
void CTextureCoordControl::LoadAlphaTexture(LPCWSTR filename)
{
	m_bshare = false ;

	D3DXCreateTextureFromFile( g_pD3DDevice , filename , &m_alphatex ) ;

	assert( m_alphatex != NULL ) ;

}

//說明：更新Texture屬性				add 08/1/21
void CTextureCoordControl::UpdateTextureAtt( void )
{
	assert( m_texture != NULL ) ;
	if(m_texture == 0)
		return;

	////////////			設定相關屬性		////////////////////
	D3DSURFACE_DESC desc ;

	m_texture->GetLevelDesc( 0 , &desc ) ;


	//設定ver's Size
	float tempx = static_cast<float>(desc.Width) ;
	float tempy = static_cast<float>(desc.Height) ;

/*
	//計算最大的放大係數
	if ( tempx > tempy )
	{
		//當寬度大於高度時，計算比例。由此比例來限制U座標
		//會乘上s_texcoordcenter的原因是，m_texminzoom記錄的是總U的一半
		m_texminzoom.x = (tempy / tempx) * s_texcoordcenter ;
		m_texminzoom.y = s_texcoordcenter ;
		//紀錄此Texture的中心點
		m_texUVcenter.x = s_texcoordcenter + m_texminzoom.x ;
	}
	else if ( tempy < tempx )
	{
		m_texminzoom.y = (tempx / tempy) * s_texcoordcenter ;
		m_texminzoom.x = s_texcoordcenter ;
		//紀錄此Texture的中心點
		m_texUVcenter.y = s_texcoordcenter + m_texminzoom.y ;
	}*/

/*
	//設定ver's Size
	m_versize.x = static_cast<float>(desc.Width) ;
	m_versize.y = static_cast<float>(desc.Height) ;

	//計算最大的放大係數
	if ( m_versize.x > m_versize.y )
	{
		//當寬度大於高度時，計算比例。由此比例來限制U座標
		//會乘上s_texcoordcenter的原因是，m_texminzoom記錄的是總U的一半
		m_texminzoom.x = (m_versize.y / m_versize.x) * s_texcoordcenter ;
		m_texminzoom.y = s_texcoordcenter ;
		//紀錄此Texture的中心點
		m_texUVcenter.x = s_texcoordcenter + m_texminzoom.x ;
	}
	else if ( m_versize.y < m_versize.x )
	{
		m_texminzoom.y = (m_versize.x / m_versize.y) * s_texcoordcenter ;
		m_texminzoom.x = s_texcoordcenter ;
		//紀錄此Texture的中心點
		m_texUVcenter.y = s_texcoordcenter + m_texminzoom.y ;
	}
*/
	
	//設定UV寬度大小(縮放到最大)
	SetTexUVSize( m_texmaxzoom , m_texmaxzoom ) ;

	//設定tex's Size
	m_texSize.x = tempx ;
	m_texSize.y = tempy ;

	
	//重新設定頂點的位置
//	SetVertexSize(  static_cast<int>(m_versize.x) , static_cast<int>(m_versize.y) ) ;
}

//說明：是否縮放到最大（鏡頭拉到最遠）
bool CTextureCoordControl::IsMaxZoomIn( void ) const 
{
	if ( m_texUV.x >= m_texminzoom.x && m_texUV.y  >= m_texminzoom.y )
	{
		return true ;
	}

	return false ;	
}

//說明：是否縮放到最小（鏡頭拉到最近）
bool CTextureCoordControl::IsMinZoomOut( void ) const
{
	if ( m_texUV.x <= m_texmaxzoom  && m_texUV.y  <= m_texmaxzoom)
	{
		return true ;
	}

	return false ;
}


//說命：取得頂點的寬與高
void CTextureCoordControl::GetVertexSize(float &width, float &height) const 
{
	width = m_versize.x ; 
	height = m_versize.y ;
}

//說明：取得頂點位置（左上點）
void CTextureCoordControl::GetVertexLeftTop( float &left , float &top ) const 
{
	left = m_verpos.x - m_versize.x / 2 ; 
	top  = m_verpos.y - m_versize.y / 2; 
}

//說明：設定頂點位置（左上點）
void CTextureCoordControl::SetVertexPos(int posx, int posy) 
{
	//偏移
	m_verpos.x = posx + (m_versize.x * 0.5f)   ;
	m_verpos.y = posy + (m_versize.y * 0.5f) ;

	//更新頂點位置
	UpdateVertex( ) ;
}

//說明：設定對Texture座標的縮小係數
void CTextureCoordControl::SetMaxZoomOut( float value )
{
	if ( value >= s_texcoordcenter )
	{
		value = s_texcoordcenter ; 
	}
	m_texmaxzoom = value ;
}

//說明：設定對Texture座標的放大係數   
void CTextureCoordControl::SetMaxZoomIn( float worldwidth , float worldheight)
{
	m_texminzoom.x = (s_worldviewrect / worldwidth) * s_texcoordcenter ;
	m_texminzoom.y = (s_worldviewrect / worldheight) * s_texcoordcenter ;
}

//說明：設定縮放的解析度
void CTextureCoordControl::ResetZoomValue( float value , float time )
{
	float zoomwidth ;
	float zoomheight ;


	//計算縮放的距離
	zoomwidth = m_texminzoom.x - m_texmaxzoom  ;
	zoomheight = m_texminzoom.y - m_texmaxzoom ;


	//因為距離相等，value隨著電腦的配備不同而改變，time是控制偏移量。
	//如：20m(zoomwidth)，花一秒鐘(value)跑完。time是控制所花的時間。
	float offset = value / time ;


	//維持矩型的比例
	m_texzoom.x = offset * zoomwidth   ; 
	m_texzoom.y = offset * zoomheight  ; 





}

//說明：設定縮放的倍數
void CTextureCoordControl::SetZoomMultiple( float multiple )
{
	//這裡是使用者直接設定解析度。不需要用perframetime來決定。
	//這裡適合滑鼠滾輪去使用。比如：multiple為6的話，只要滾動
	//個6次就到達極限了。
	m_texzoommultiple = multiple ;

	//只要縮放倍數更新時，就要重新縮放係數
	m_texzoom.x = (m_texminzoom.x - m_texmaxzoom) / multiple ;
	m_texzoom.y = (m_texminzoom.y - m_texmaxzoom) / multiple ;
}


//說明：設定頂點的寬跟高度
void CTextureCoordControl::SetVertexSize(int sx, int sy)
{

	D3DXVECTOR2 half ;

	//更新頂點size
	m_versize.x = static_cast<float>(sx) ;
	m_versize.y = static_cast<float>(sy) ;

	half.x = static_cast<float>(sx) ;//寬
	half.y = static_cast<float>(sy) ;//高

	D3DXVec2Scale( &half , &half , 0.5f ) ;

	//這是螢幕座標系

	//左上
	m_verbasepos[0].x = -half.x ;
	m_verbasepos[0].y = -half.y ;
	//左下
	m_verbasepos[1].x = -half.x ;
	m_verbasepos[1].y = half.y ;
	//右下
	m_verbasepos[2].x = half.x ;
	m_verbasepos[2].y = half.y ;
	//右上
	m_verbasepos[3].x = half.x ;
	m_verbasepos[3].y = -half.y ;

	//更新頂點位置
	UpdateVertex( ) ;

}

//說明：累加UV座標位置
void CTextureCoordControl::AddUVTranslate(float offsetx, float offsety)
{

	//UV的中心點
	float u = m_texUVpos.x ;
	float v = m_texUVpos.y ;

	//以中心點為基準，更新TexRect座標
	UpdateUVCoord( u , v , m_texUV.x , m_texUV.y ) ;

	float tempu = -1.0f ;
	float tempv = -1.0f ;

	//檢查RECT範圍。(tempu , tempv)就是超過 0.0 <= value <= 1.0f的偏移量
	//然後去修正（算出來的tempu , tempv）
	if ( CheckInside( m_texUVleft , m_texUVtop , m_texUVright , m_texUVbottom , 0.0f  , s_texcoordrightbottom  , tempu , tempv ) )
	{
		//////////////		會超過的特別原因：因為縮小Texture時，然後移動到矩型的某一個底線，
		/////////////						  再次縮小Texture時，在移動到矩型的此方向的底線，
		////////////						  然後在放大Texture，會造成超出 0.0 <= value <= s_texcoordrightbottom範圍，
		///////////							  這是UpdateZoomCenter的關係。在這邊作個修正。

		//如果有改變的話，進行修正
		if ( ( tempu + 1.0f ) >= 0.0001f )
		{
			//判斷作哪一方向的修正
			if ( m_texUVpos.x >= s_texcoordcenter )
			{
				m_texUVpos.x = m_texUVpos.x - tempu ;
			}
			else
			{
				m_texUVpos.x = m_texUVpos.x + tempu;
			}
			
		}
		if ( ( tempv + 1.0f ) >= 0.0001f )
		{
			//判斷作哪一方向的修正
			if ( m_texUVpos.y >= s_texcoordcenter )
			{
				m_texUVpos.y = m_texUVpos.y - tempv ;
			}
			else
			{
				m_texUVpos.y = m_texUVpos.y + tempv ;
			}
		}		
	}
	else
	{
		//			以u,v為中心點，去偏移m_texUV，則決定m_texUVRect的範圍。			////////
		//			以下是判斷是否還有多餘的空間，去作偏移。						///////

		//計算當往左或上邊偏移時
		if ( offsetx < 0.0f || offsety < 0.0f  )
		{
			//判斷u是否還有多餘m_texUV的空間去偏移。如果有的話，就偏移
			if ( u > 0.0f && u - m_texUV.x > 0.0f )
			{
				//更新UV座標
				m_texUVpos.x += offsetx ;
			}
			if ( v > 0.0f && v - m_texUV.y  > 0.0f )
			{
				m_texUVpos.y += offsety ;
			}
		}
		//計算當往右或下邊偏移時
		if ( offsetx > 0.0f || offsety > 0.0f )
		{
			//判斷u是否還有多餘u + m_texUV的空間去偏移。如果有的話，就偏移
			if ( u > 0.0f && u + m_texUV.x < s_texcoordrightbottom )
			{
				//更新UV座標
				m_texUVpos.x += offsetx ;
			}
			if ( v > 0.0f && v + m_texUV.y  < s_texcoordrightbottom )
			{
				m_texUVpos.y += offsety ;
			}
		}
		//除了，改變中心位置之外，還要改變UV's Rect座標
		UpdateUVCoord( m_texUVpos.x , m_texUVpos.y , m_texUV.x , m_texUV.y ) ;
	}

	//更新texUV位置
	UpdateVertex( ) ;

}

//請輸入value <= 0.5f 。
//說明：設定貼圖座標寬與高。假設總寬度為1.0。那要設定的u's size為0.5，高度也一樣。請注意！
void CTextureCoordControl::SetTexUVSize(float u, float v)
{
	m_texUV.x = u ;
	m_texUV.y = v ;

	//更新頂點相關屬性
	UpdateVertex( ) ;
}

//說明：設定貼圖座標位置
void CTextureCoordControl::SetTexUVPos( float u , float v )
{
	//以中心點為基準，暫存TexRect座標

	UpdateUVCoord( u , v , m_texUV.x , m_texUV.y ) ;

	m_texUVpos.x = u ;
	m_texUVpos.y = v ;

	
	//更新頂點相關屬性
	UpdateVertex( ) ;
}

void CTextureCoordControl::SetRotation(float rot)
{	
	s_rotation = rot ; 

	s_texcoordcenter = cos(DegreesToRads(-s_rotation));  
	s_texcoordrightbottom = s_texcoordcenter * 2.0f; 
}

float CTextureCoordControl::GetRotation(void)
{
	return s_rotation ; 
}


//說明：設定UV座標位置
void CTextureCoordControl::SetUVTranslate(float x, float y)
{
	m_texUVpos.x = x ;
	m_texUVpos.y = y ;

	//更新texUV位置
	UpdateVertex( ) ;

}


//說明：更新頂點相關屬性
void CTextureCoordControl::UpdateVertex( void )
{	

	///////////////////				設定頂點位置			///////////////////
	//左上
	float vx0 = m_verpos.x + m_verbasepos[0].x ;
	float vy0 = m_verpos.y + m_verbasepos[0].y ;
	//左下
	float vx1 = m_verpos.x + m_verbasepos[1].x  ;
	float vy1 = m_verpos.y + m_verbasepos[1].y  ;
	//右下
	float vx2 = m_verpos.x + m_verbasepos[2].x  ;
	float vy2 = m_verpos.y + m_verbasepos[2].y ;
	//右上
	float vx3 = m_verpos.x + m_verbasepos[3].x  ;
	float vy3 = m_verpos.y + m_verbasepos[3].y ;


	//UV的中心點
	float u = m_texUVpos.x ;
	float v = m_texUVpos.y ;

	//並且更新縮放中心點。但是m_texUVpos不變喔！
	UpdateZoomCenter( u , v ) ;

	m_cameraUV.x = u ; 
	m_cameraUV.y = v ; 


	//更新UV的四點座標
	UpdateUVCoord( u , v , m_texUV.x , m_texUV.y ) ;




	float newUVLeftTopX , newUVLeftTopY , newUVLeftBottomX , newUVLeftBottomY ; 
	float newUVRightBottomX , newUVRightBottomY , newUVRightTopX , newUVRightTopY ; 

	m_texUVleft -= u ;  
	m_texUVtop -= v ; 
	m_texUVright -= u ;
	m_texUVbottom -=v ; 

	float cosAngle = cos(DegreesToRads(-CTextureCoordControl::GetRotation())); 
	float sinAngle = sin(DegreesToRads(-CTextureCoordControl::GetRotation())); 


	//左上
	newUVLeftTopX = u + (m_texUVleft * cosAngle - m_texUVtop * sinAngle) ; 
	newUVLeftTopY = v + (m_texUVleft * sinAngle + m_texUVtop * cosAngle) ; 

	//左下
	newUVLeftBottomX = u + (m_texUVleft * cosAngle - m_texUVbottom * sinAngle) ; 
	newUVLeftBottomY = v + (m_texUVleft * sinAngle + m_texUVbottom * cosAngle) ; 

	//右下
	newUVRightBottomX = u + (m_texUVright * cosAngle - m_texUVbottom * sinAngle) ; 
	newUVRightBottomY = v + (m_texUVright * sinAngle + m_texUVbottom * cosAngle) ; 


	//右上
	newUVRightTopX = u + (m_texUVright * cosAngle - m_texUVtop * sinAngle) ; 
	newUVRightTopY = v + (m_texUVright * sinAngle + m_texUVtop * cosAngle) ; 

	//更新UV的四點座標
	m_texUVleft +=  u;
	m_texUVtop += v;
	m_texUVright +=  u;
	m_texUVbottom +=  v ;



	////////////////			設定UV位置			/////////////////////////
	//SetVertex( &m_mapvertex[0] , vx0 , vy0 , 0.0f , 1.0f , m_texUVleft  , m_texUVtop , 0.0f , 0.0f) ;
	//SetVertex( &m_mapvertex[1] , vx1 , vy1 , 0.0f , 1.0f , m_texUVleft  , m_texUVbottom , 0.0f , 1.0f ) ;
	//SetVertex( &m_mapvertex[2] , vx2 , vy2 , 0.0f , 1.0f , m_texUVright  ,m_texUVbottom , 1.0f , 1.0f  ) ;
	//SetVertex( &m_mapvertex[3] , vx3 , vy3 , 0.0f , 1.0f , m_texUVright , m_texUVtop , 1.0f , 0.0f ) ;

	SetVertex( &m_mapvertex[0] , vx0 , vy0 , 0.0f , 1.0f , newUVLeftTopX  , newUVLeftTopY , 0.0f , 0.0f) ;
	SetVertex( &m_mapvertex[1] , vx1 , vy1 , 0.0f , 1.0f , newUVLeftBottomX  , newUVLeftBottomY , 0.0f , 1.0f ) ;
	SetVertex( &m_mapvertex[2] , vx2 , vy2 , 0.0f , 1.0f , newUVRightBottomX  ,newUVRightBottomY , 1.0f , 1.0f  ) ;
	SetVertex( &m_mapvertex[3] , vx3 , vy3 , 0.0f , 1.0f , newUVRightTopX , newUVRightTopY , 1.0f , 0.0f ) ;

}

//說明：更新縮放中心點
void CTextureCoordControl::UpdateZoomCenter(float &u, float &v)
{

	////				以(0.5f , 0.5f)為中心點		//////////////////

	//主要觀念：縮放中心必須隨時注意到，周圍的四個座標位置，保持m_texUV的寬與高之外，
	//還要注意到，不能 小於0 或 大於1.0f 的貼圖座標範圍。

	//所以，第一個if判斷式判斷(u ,v)在第幾像限。
	//裡面的兩個判斷式：當 u < 0.5f 的狀況下，u <= m_texUV.x 時，表示不能保持m_texUV的寬，所以直接帶入m_texUV的寬
	//					當 u > 0.5f 的狀況下，u + m_texUV.x > 1.0f ，就是要判斷是否超過貼圖座標的最大值，
	//					如果超過的話，將中心的u 扣掉此值，就不會超過1.0f了。不瞭解的話，請畫圖吧！

	//最後，只要抓出其中一個象限就行了 

	//但是現在下面的程序，中心點不是固定在(0.5f , 0.5f)了


	//第2像限
	if ( u < s_texcoordcenter && v < s_texcoordcenter )
	{
		if ( u <= m_texUV.x )
		{
			u = m_texUV.x ;
		}
		if ( v <= m_texUV.y )
		{
			v = m_texUV.y ;
		}

		return ;
	}

	//第1像限
	if ( u >s_texcoordcenter && v < s_texcoordcenter )
	{
		float tempU = u + m_texUV.x ;

		if ( tempU >= s_texcoordrightbottom)
		{
			u -= (tempU - s_texcoordrightbottom) ;
		}
		if ( v <= m_texUV.y )
		{
			v = m_texUV.y ;
		}

		return ;
	}

	//第3像限
	if ( u < s_texcoordcenter && v > s_texcoordcenter )
	{
		float tempV = v + m_texUV.y ;

		if ( u <= m_texUV.x )
		{
			u = m_texUV.x ;
		}
		if ( tempV >= s_texcoordrightbottom)
		{
			v -= (tempV - s_texcoordrightbottom) ;
		}

		return ;
	}	

	//第4像限
	if ( u > s_texcoordcenter && v > s_texcoordcenter )
	{
		float tempU = u + m_texUV.x ;
		float tempV = v + m_texUV.y ;

		if ( tempU >= s_texcoordrightbottom)
		{
			u -= (tempU - s_texcoordrightbottom);
		}
		if ( tempV >= s_texcoordrightbottom)
		{
			v -= (tempV - s_texcoordrightbottom) ;
		}

		return ;
	}


	//右
	if ( u > s_texcoordcenter && ((v - s_texcoordcenter) < EPSINON) )
	{
		float tempU = u + m_texUV.x ;

		if ( tempU >= s_texcoordrightbottom)
		{
			u -= (tempU - s_texcoordrightbottom) ;
		}

		return ;
	}

	//上
	if ( ((u - s_texcoordcenter) < EPSINON)  && v < s_texcoordcenter)
	{
		if ( v <= m_texUV.y )
		{
		   v = m_texUV.y ;
		}

		return ;
	}

	//左
	if ( u < s_texcoordcenter && ((v - s_texcoordcenter) < EPSINON) )
	{
		if ( u <= m_texUV.x )
		{
			u = m_texUV.x ;
		}

		return ;
	}

	//下
	if ( ((u - s_texcoordcenter) < EPSINON)  && v > s_texcoordcenter)
	{
		float tempV = v + m_texUV.y ;

		if ( tempV >= s_texcoordrightbottom)
		{
			v -= (tempV - s_texcoordrightbottom) ;
		}

		return ;
	}
}

//說明：更新貼圖座標的四個點
void CTextureCoordControl::UpdateUVCoord( float centeru , float centerv, float uwidth , float vheight )
{
	//更新UV的四點座標
	m_texUVleft = centeru - uwidth ;
	m_texUVtop = centerv - vheight ;
	m_texUVright = centeru + uwidth ;
	m_texUVbottom =  centerv + vheight ;

}

//參數：第1.2參數範圍為0~1
bool CTextureCoordControl::GetRotUVCoord(float inScaleX , float inScaleY , float &outU , float &outV , 
										 float &outCenterX ,float &outCenterY)
{

	outCenterX = m_cameraUV.x  ; 
	outCenterY = m_cameraUV.y ; 



	D3DXVECTOR2 toCenter ; 

	toCenter.x = m_texUVleft + (m_texUVright - m_texUVleft) * inScaleX ; 
	toCenter.y = m_texUVtop + (m_texUVbottom - m_texUVtop) * inScaleY ; 

	toCenter.x -= m_cameraUV.x ; 
	toCenter.y -= m_cameraUV.y ; 


	float dist = D3DXVec2Length(&toCenter) ; 

	D3DXVec2Normalize(&toCenter , &toCenter) ; 


	float cosAngle = cos(DegreesToRads(CTextureCoordControl::GetRotation())); 
	float sinAngle = sin(DegreesToRads(CTextureCoordControl::GetRotation()));

	outU = toCenter.x * cosAngle - toCenter.y * -sinAngle ; 
	outV = toCenter.x * -sinAngle + toCenter.y * cosAngle ;

	outU =  m_cameraUV.x + outU * dist; 
	outV = m_cameraUV.y + outV *  dist ; 


	if(outU < 0.0f)
	{
		outU = 0.0f ; 
	}
	else if(outU > 1.0f)
	{
		outU = 1.0f ; 
	}

	if(outV < 0.0f)
	{
		outV = 0.0f ; 
	}
	else if(outV > 1.0f)
	{
		outV = 1.0f ; 
	}
	return true ;
}

//說明：檢查RECT的座標，是否有超過min和max的限制範圍。如果有的話，回傳true
bool CTextureCoordControl::CheckInside( float &left , float &top , float &right , float &bottom , float min , 
									float max , float &u , float &v)
{
	bool chang = false ;

	//左邊UV座標限制
	if ( left < min )
	{
		u = -left ;

		left = min ;

		chang = true ;
	}
	//右邊UV座標限制
	if ( right > max )
	{
		u = right - max ;

		right = max ;

		chang = true ;
	}
	//上面UV座標限制
	if ( top < min )
	{
		v = -top ;

		top = min ;

		chang = true ;
	}
	//下面UV座標限制
	if ( bottom > max )
	{
		v = bottom - max ;

		bottom = max ; 

		chang = true ;
	}
	
	return chang ;
}

//說明：判斷此貼圖座標是否在顯示貼圖四座標的範圍之內
bool CTextureCoordControl::IsInsideForShowUV(float u, float v)
{

	//內！
	if ( u >= (m_texUVleft ) &&
		u < (m_texUVright )  &&
		v >= (m_texUVtop)  &&
		v <= (m_texUVbottom ) )
	{
		return true ;
	}

   //外！
   return false;
	
}




//說明：設定VERTEX2D相關屬性
void CTextureCoordControl::SetVertex( VERTEX2D *vec , float x , float y , float z , 
								  float rhw , float u0 , float v0 , float u1 , float v1 )
{
	vec->x = x ;
	vec->y = y ;
	vec->z = z ;
	vec->rhw = rhw ;
	vec->u0 = u0 ;
	vec->v0 = v0 ;
	vec->u1 = u1 ;
	vec->v1 = v1 ;

}


bool CTextureCoordControl::MessageHandle(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
/*	switch ( uMsg )
	{
	case WM_LBUTTONUP : //放開左鍵，設定頂點位置
		{
			int curX = LOWORD(lParam) ;
			int curY = HIWORD(lParam) ;

			SetVertexPos( static_cast<float>(curX) , static_cast<float>(curY) ) ;
		}break ;
	case WM_MOUSEWHEEL: //滑鼠滾輪訊息
		{
			int delta = static_cast<int>(wParam) ;

			//向上滾動時
			if ( delta > 0 )
			{
				//縮小
				TexZoomIn( ) ;
			}
			//向下滾動時
			else if ( delta < 0 ) 
			{
				//放大
				TexZoomOut( ) ;
			}
		}break ;*/
	/////////////////	鍵盤	/////////////
 /*   case WM_KEYUP:
        {
            switch( wParam )
            {
				case VK_LEFT:
				{
					//要注意此偏移量的精準度
					AddUVTranslate( -MOVEOFFSET , 0.0f ) ;
					return true ;
				}
				case VK_UP:
				{
					AddUVTranslate( 0.0f , -MOVEOFFSET ) ;

					return true ;
				}
				case VK_DOWN:
				{
					AddUVTranslate( 0.0f , MOVEOFFSET ) ;

					return true ;
				}
				case VK_RIGHT:
				{
					AddUVTranslate( MOVEOFFSET , 0.0f ) ;
					return true ;
				}
				case 107 :  // "+"鍵
				{
					TexZoomOutMax( ) ;
					return true ;
				}
				case 109 : // "-"鍵
				{
					TexZoomInMax( ) ;
					return true ;
				}
            }break;
        }
	default:
		break ;
	};*/
	return true ;
}



//說明：縮小Texture's uv
void CTextureCoordControl::TexZoomIn( void )
{
	//設定縮小的話，意思說：加大UV的座標
	m_texUV.x += m_texzoom.x ;
	m_texUV.y += m_texzoom.y ;

	//限制UV座標
	if ( m_texUV.x >= m_texminzoom.x )
	{
		m_texUV.x = m_texminzoom.x ;
	}
	if ( m_texUV.y >= m_texminzoom.y)
	{
		m_texUV.y = m_texminzoom.y ;
	}

	//更新UV座標
	UpdateVertex( ) ;
}

//說明：放大Texture' uv 
void CTextureCoordControl::TexZoomOut( void )
{
	//設定放大的話，意思說：減少UV的座標
	m_texUV.x -= m_texzoom.x ;
	m_texUV.y -= m_texzoom.y ;


	//限制UV座標
	if ( m_texUV.x <= m_texmaxzoom )
	{
		m_texUV.x = m_texmaxzoom ;
	}
	if ( m_texUV.y <= m_texmaxzoom )
	{
		m_texUV.y = m_texmaxzoom ;
	}

	//更新UV座標
	UpdateVertex( ) ;
}

//說明：縮小到最大極限
void CTextureCoordControl::TexZoomInMax( void )
{	
	m_texUV.x = m_texminzoom.x ;
	m_texUV.y = m_texminzoom.y ;

	//更新UV座標
	UpdateVertex( ) ;		
}

//說明：放大到最大極限
void CTextureCoordControl::TexZoomOutMax( void )
{
	m_texUV.x = m_texmaxzoom ;
	m_texUV.y = m_texmaxzoom ;

	//更新UV座標
	UpdateVertex( ) ;		
	
}

//說明：使用aplhaTex
void CTextureCoordControl::DrawTexWithAlphaTex(IDirect3DDevice9 *d3d)
{

	assert( m_alphatex != NULL ) ;
	assert( m_texture != NULL ) ;

	//打開alpha通道
	d3d->SetRenderState( D3DRS_ALPHABLENDENABLE , true ) ;

	//render
	d3d->SetFVF(MY_FVF_VERTEX);
	
	d3d->SetTexture( 0 , m_texture ) ;
	d3d->SetTexture( 1 , m_alphatex ) ;


	//將texture and diffuse 作alpha混色  
	d3d->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_MODULATE    ) ;
	d3d->SetTextureStageState( 0 , D3DTSS_ALPHAARG1  , D3DTA_TEXTURE ) ;
	d3d->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_DIFFUSE ) ;

	//將texture and diffuse 作color混色
	d3d->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_MODULATE   ) ;
	d3d->SetTextureStageState( 0 , D3DTSS_COLORARG1 , D3DTA_TEXTURE ) ;
	d3d->SetTextureStageState( 0 , D3DTSS_COLORARG2 , D3DTA_DIFFUSE ) ;

	//作alpha混色  
	d3d->SetTextureStageState( 1 , D3DTSS_ALPHAARG1  , D3DTA_TEXTURE ) ;
	d3d->SetTextureStageState( 1 , D3DTSS_ALPHAARG2 , D3DTA_CURRENT ) ;
	d3d->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_MODULATE   ) ;

	//作color混色
	d3d->SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_ADD ) ;
	d3d->SetTextureStageState( 1 , D3DTSS_COLORARG1 , D3DTA_TEXTURE ) ;
	d3d->SetTextureStageState( 1 , D3DTSS_COLORARG2 , D3DTA_CURRENT ) ;

	// Set the texture stages for the second texture unit (image).
    d3d->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	d3d->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);


	d3d->SetRenderState( D3DRS_SRCBLEND  , D3DBLEND_SRCALPHA ) ;
	d3d->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA ) ;

	d3d->DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_mapvertex , sizeof(VERTEX2D) ) ;


	d3d->SetTexture( 0 , NULL ) ;
	d3d->SetTexture( 1 , NULL ) ;

	d3d->SetRenderState( D3DRS_ALPHABLENDENABLE , false ) ;

}

//說明：不使用aplhaTex
void CTextureCoordControl::DrawTex(IDirect3DDevice9 *d3d)
{
	assert( m_texture != NULL ) ;

	//render
	d3d->SetFVF(MY_FVF_VERTEX);

	//將texture and diffuse 作alpha混色  
	//d3d->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG1    ) ;
	//d3d->SetTextureStageState( 0 , D3DTSS_ALPHAARG1  , D3DTA_TEXTURE ) ;
	//d3d->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_DIFFUSE ) ;

	////將texture and diffuse 作color混色
	//d3d->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_SELECTARG1   ) ;
	//d3d->SetTextureStageState( 0 , D3DTSS_COLORARG1 , D3DTA_TEXTURE ) ;
	//d3d->SetTextureStageState( 0 , D3DTSS_COLORARG2 , D3DTA_DIFFUSE ) ;

	d3d->SetSamplerState(0 , D3DSAMP_ADDRESSU   , D3DTADDRESS_BORDER) ; 
	d3d->SetSamplerState(0 , D3DSAMP_ADDRESSV  , D3DTADDRESS_BORDER) ; 
	d3d->SetSamplerState(0 , D3DSAMP_BORDERCOLOR  , 0x00000000) ; 
	
	d3d->SetTexture( 0 , m_texture ) ;

	d3d->DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_mapvertex , sizeof(VERTEX2D) ) ;

	d3d->SetTexture( 0 , NULL ) ;

}



////
//		類別：小地圖控制
//		註明：繼承自CDXUTControl
////

CMapControl::CMapControl( CDXUTDialog *pDialog ) : CDXUTControl( pDialog ) 
{

	m_pDialog = pDialog ; 

	Init( ) ;

}

CMapControl::~CMapControl( void )
{
	Release( ) ;
}


void CMapControl::AddUVTranslate( float offsetx , float offsety )
{
	assert( m_pmap != NULL ) ;

	m_pmap->AddUVTranslate( offsetx , offsety ) ;

	//更新視窗屬性
	UpdateView( ) ;
}

void CMapControl::SetWindowPos( int posx , int posy )
{
	assert( m_pmap != NULL ) ;

	//先取得頂點位置(此時VerSize早已經設定完畢)
	m_pmap->SetVertexPos( posx , posy ) ;

	//更新視窗屬性
	UpdateView( ) ;

}

//說明：取得人物在螢幕座標位置
void CMapControl::GetRoleViewcoord(int &posx, int &posy) const
{
	
	posx = static_cast<int>(m_roleposforview.x)  ;
	posy = static_cast<int>(m_roleposforview.y)  ;
}


void CMapControl::SetWindowSize( int sx , int sy )
{
	assert( m_pmap != NULL ) ;

	m_pmap->SetVertexSize( sx , sy ) ;

	//更新視窗屬性
	UpdateView( ) ;

}

//說明：設定縮放間格
void CMapControl::SetZoomParam( float zoomtime )
{
	//這數值表示：從一個縮放點到另一個縮放點，所花費的時間。
	m_zoomtime = zoomtime ;

}

void CMapControl::SetMaxZoomOut(float value)
{
	assert( m_pmap != NULL ) ;

	m_pmap->SetMaxZoomOut( value ) ;
}

//說明：放大
void CMapControl::TexZoomIn( void )
{
	assert( m_pmap != NULL ) ;

	//縮放之前，設定UV座標為人物的Texture位置
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;

	//再進行放大
	m_pmap->TexZoomIn( ) ;

	//在更新視窗屬性
	UpdateView( ) ;


	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
	m_roleposforview.x , m_roleposforview.y ) ;
}

//說明：放到最大
void CMapControl::TexZoomInMax( void )
{
	assert( m_pmap != NULL ) ;

	//縮放之前，設定UV座標為人物的Texture位置。
	//因為縮放是以人物為基準。
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;
	
	m_pmap->TexZoomInMax( ) ;

	//更新視窗屬性
	UpdateView( ) ;

	//轉換人物座標到視窗座標
	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
	m_roleposforview.x , m_roleposforview.y ) ;


}

//說明：縮小
void CMapControl::TexZoomOut( void )
{
	assert( m_pmap != NULL ) ;

	//縮放之前，設定UV座標為人物的Texture位置。
	//因為縮放是以人物為基準。
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;

	m_pmap->TexZoomOut( ) ;

	//更新視窗屬性
	UpdateView( ) ;

	//轉換人物座標到視窗座標
	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
	m_roleposforview.x , m_roleposforview.y ) ;
}

//說明：縮到最小
void CMapControl::TexZoomOutMax( void )
{
	assert( m_pmap != NULL ) ;

	//縮放之前，設定UV座標為人物的Texture位置。
	//因為縮放是以人物為基準。
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;

	m_pmap->TexZoomOutMax( ) ;

	//轉換人物座標到視窗座標
	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
	m_roleposforview.x , m_roleposforview.y ) ;
}

void CMapControl::SetTexUVPos( float u , float v )
{
	assert( m_pmap != NULL ) ;

	m_pmap->SetTexUVPos( u , v ) ;

	//更新視窗屬性
	UpdateView( ) ;
}

void CMapControl::SetMapFile(LPCWSTR filename)
{
	assert( m_pmap != NULL ) ;

	// >> add 08/1/21
	CDXUTElement* pElement = NULL ;
	CDXUTDialogResourceManager* pManager = NULL ;
	DXUTTextureNode *pnode = NULL ;

	//讀圖
	CDXUTControl::SetTexture( filename , NULL , 0 ) ;
	//抓元件
	pElement = CDXUTControl::GetElement( 0 ) ;
	
	assert( pElement != NULL ) ;
	//抓資源
	pManager = m_pDialog->GetManager() ;

	assert( pManager != NULL ) ;
	
	//抓Texture's node
	pnode = pManager->GetTextureNode( pElement->iTexture );

	//丟Texture
	m_pmap->SetTexture( pnode->pTexture ) ;
	// << add 08/1/21


	
	//m_pmap->LoadTexture( filename ) ;

	//更新小地圖大小
	m_pmap->GetTextureSize( m_texsize.x , m_texsize.y ) ;

}

void CMapControl::SetMapMaskFile(LPCWSTR filename)
{
	assert( m_pmap != NULL ) ;
	// >> add 08/1/21
	CDXUTElement* pElement = NULL ;
	CDXUTDialogResourceManager* pManager = NULL ;
	DXUTTextureNode *pnode = NULL ;

	//讀圖
	CDXUTControl::SetTexture( filename , NULL , 1 ) ;
	//抓元件
	pElement = CDXUTControl::GetElement( 1 ) ;
	
	assert( pElement != NULL ) ;
	//抓資源
	pManager = m_pDialog->GetManager() ;

	assert( pManager != NULL ) ;
	
	//抓Texture's node
	pnode = pManager->GetTextureNode( pElement->iTexture );

	//丟AlphaTexture
	m_pmap->SetAlphaTexture(pnode->pTexture ) ;

	

//	m_pmap->LoadAlphaTexture( filename ) ;

}

void CMapControl::Render(IDirect3DDevice9 *d3d , float fElapsedTime)
{
	assert( m_pmap != NULL ) ;
	assert( d3d != NULL ) ;


//	CDXUTDialog::GetManager()->m_pSprite->End();

	CDXUTDialog::GetManager()->m_pSprite->Flush() ; 

	UpdateZoomTime( fElapsedTime ) ;

	UpdateRects( ) ;


	
	m_pmap->DrawTex( d3d ) ;



//	CDXUTDialog::GetManager()->m_pSprite->Begin( D3DXSPRITE_DONOTSAVESTATE );

}

void CMapControl::RenderWithAlphaTex(IDirect3DDevice9 *d3d , float fElapsedTime)
{
	assert( m_pmap != NULL ) ;
	assert( d3d != NULL ) ;

	UpdateZoomTime( fElapsedTime ) ;

	m_pmap->DrawTexWithAlphaTex( d3d ) ;


}

bool CMapControl::MessageHandle( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{
	/*
	switch ( uMsg )
	{
	case WM_LBUTTONDOWN : //按下左鍵，縮放
		{
			//設定放大狀態。
			SetZoomState( ZOOMIN ) ;
		}break ;
	case WM_LBUTTONUP : //放開左鍵，取消縮放
		{
			//設定放大狀態。
			SetZoomState( ZOOMNULL ) ;
		}break ;
	case WM_RBUTTONDOWN:  //按下右鍵，縮放
		{
			//設定縮小狀態。
			SetZoomState( ZOOMOUT ) ;
		}break ;
	case WM_RBUTTONUP:  //放開右鍵，取消縮放
		{
			//設定縮小狀態。
			SetZoomState( ZOOMNULL ) ;
		}break ;
	////////////////	鍵盤	/////////////
    case WM_KEYUP:
        {
            switch( wParam )
            {
				case VK_LEFT:
				{
					m_roleworldpos.x -= 5.0f ; 

					SetRoleWorldPos( m_roleworldpos.x , m_roleworldpos.y ) ;
					return true ;
				}
				case VK_UP:
				{
					m_roleworldpos.y -= 5.0f ; 

					SetRoleWorldPos( m_roleworldpos.x , m_roleworldpos.y ) ;

					return true ;
				}
				case VK_DOWN:
				{

					m_roleworldpos.y += 5.0f ; 

					SetRoleWorldPos( m_roleworldpos.x , m_roleworldpos.y ) ;


					return true ;
				}
				case VK_RIGHT:
				{

					m_roleworldpos.x += 5.0f ; 

					SetRoleWorldPos( m_roleworldpos.x , m_roleworldpos.y ) ;

					AddRoleWorldPos( 5 , 0.0f ) ;
					return true ;
				}
				case 107 :  // "+"鍵
				{
					TexZoomOutMax( ) ;
					return true ;
				}
				case 109 : // "-"鍵
				{
					TexZoomInMax( ) ;
					return true ;
				}
            }break;
        }
	default:
		break ;
	};*/
	return true ;
	
}

void CMapControl::Init( void )
{
	m_pmap = new CTextureCoordControl ;

	assert( m_pmap != NULL ) ;

	memset( &m_viewposfortex , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_viewposforstart , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_texsize , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_viewsize , 0 , sizeof(D3DXVECTOR2) ) ;

	memset( &m_viewposfortexrightbottom , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_viewposfortexlefttop , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_viewsizefortexlefttop , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_viewsizefortexrightbottom , 0 , sizeof(D3DXVECTOR2) ) ;



	memset( &m_roleposforview , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_roleposfortex , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_rolefortex , 0 , sizeof(D3DXVECTOR2) ) ;

	memset( &m_worldsize , 0 , sizeof(D3DXVECTOR2) ) ;
	memset( &m_wroldforstart , 0 , sizeof(D3DXVECTOR2) ) ;

	memset( &m_worldtomapscale , 0 , sizeof(D3DXVECTOR2) ) ;

	m_zoomtime = 1.0f ;
	m_zoomstate = ZOOMNULL ;


	//初始GUI的Type
	m_Type = DXUT_CONTROL_MAPCONTROL ;


}

void CMapControl::Release( void )
{
	delete m_pmap ;
	m_pmap = NULL ;


}


//參數：，世界地圖的寬與高，世界地圖的起始座標(x,z)，預設值為(0 , 0)
//說明：
void CMapControl::ResetMapAtt( float worldwidth, float worldheight , float x , float z  )
{
	assert( m_pmap != NULL ) ;

	m_worldsize.x = worldwidth ;
	m_worldsize.y = worldheight ;

	m_wroldforstart.x = x ;
	m_wroldforstart.y = z ;

	//利用世界的寬度與高度，來計算最大的縮放係數 
	m_pmap->SetMaxZoomIn( worldwidth , worldheight) ;

	//更新比例
	UpdateScale( ) ;

}

//說明：設定縮放狀態
void CMapControl::SetZoomState( ZoomState state )
{
	m_zoomstate = state ; 
}

//說明：取得縮放的狀態
ZoomState CMapControl::GetZoomState( void ) const 
{
	return m_zoomstate ;
}

//說明：更新縮放時間
void CMapControl::UpdateZoomTime( float fElapsedTime )
{

	//是無縮放狀態的話
	if ( m_zoomstate == ZOOMNULL  )
	{
		return ;
	}

	assert( m_pmap != NULL ) ;

	//重新設定縮放係數。
	m_pmap->ResetZoomValue( fElapsedTime , m_zoomtime ) ;
	//進行縮放的動作
	Zoom( ) ;
	
}

//說明：進行縮放(不管是小或是大)
void CMapControl::Zoom( void )
{
	switch ( m_zoomstate )
	{
	case ZOOMIN:
		{
			TexZoomIn( ) ;
		}break ;
	case ZOOMOUT:
		{
			TexZoomOut( ) ;
		}break ;
	case ZOOMNULL:
		{
		}break ;
	default:
		{
		}break ;
	};

}

//說明：計算世界座標 對 小地圖座標的比例
void CMapControl::UpdateScale( void )
{
	float widthscale ;
	float heightscale ;
	
	//世界size 跟 小地圖size 的比例
	widthscale = m_worldsize.x / m_texsize.x ;
	heightscale = m_worldsize.y / m_texsize.y ;
	//世界的座標的pixel 跟 小地圖的pixel 的比例
	m_worldtomapscale.x = (1 / widthscale) / m_texsize.x ;
	m_worldtomapscale.y = (1 / heightscale) / m_texsize.y ;

}

//說明：更新視窗的大小和位置(依造父類別)			add 1/21
void CMapControl::UpdateRects( void )
{
	//更新父類別元件的Rect 
	CDXUTControl::UpdateRects( ) ;

	assert( m_pmap != NULL ) ;

	if ( m_pDialog == NULL )
	{
		return ;
	}
	
	POINT point ;
	int tempx ;
	int tempy ;

	m_pDialog->GetLocation( point ) ;

	tempx = point.x + m_rcBoundingBox.left;
	tempy = point.y + m_rcBoundingBox.top   ;

	//更新位置
	SetWindowPos(  tempx , tempy  ) ;

}

//說明：更新視窗的相關屬性
void CMapControl::UpdateView( void )
{
	assert( m_pmap != NULL ) ;

	//取得頂點左上位置
	m_pmap->GetVertexLeftTop( m_viewposforstart.x , m_viewposforstart.y ) ;

	//更新頂點Size屬性
	m_pmap->GetVertexSize( m_viewsize.x , m_viewsize.y ) ;

	//隨時保持更新viewposfortex
	m_pmap->GetUVpos( m_viewposfortex.x , m_viewposfortex.y ) ;
	//隨時保持更新UV的Rect座標
	m_pmap->GetUVposRect( m_viewposfortexlefttop.x   ,  m_viewposfortexlefttop.y , 
						  m_viewposfortexrightbottom.x , m_viewposfortexrightbottom.y ) ;

	//更新視窗對應Texture的範圍大小。
	//如：(256 , 256)的右下角視窗大小，是在Texture中的(1024 , 512)的位置
	m_viewsizefortexrightbottom.x = m_viewposfortexrightbottom.x * m_texsize.x ;
	m_viewsizefortexrightbottom.y = m_viewposfortexrightbottom.y * m_texsize.y ;
	//這是左上角部分
	m_viewsizefortexlefttop.x = m_viewposfortexlefttop.x * m_texsize.x ;
	m_viewsizefortexlefttop.y = m_viewposfortexlefttop.y * m_texsize.y ;
}

//參數：世界座標（in），貼圖座標（out），Texture的Pixel座標（out）
//說明：利用世界座標，求出貼圖座標和視窗座標
void CMapControl::Transform(float worldx, float worldz, float &texu, float &texv,
							float &pixelx, float &pixely)
{
	float rolex ;
	float roley ;

	//計算之後：role座標會在(0~256)之間
	rolex = worldx - m_wroldforstart.x ;
	roley = worldz - m_wroldforstart.y ;

	//計算人物在texture's coord的範圍
	texu =  m_worldtomapscale.x * rolex ;
	texv =  m_worldtomapscale.y * roley ;


	//記算人物在texture中的pixel位置
	pixelx = texu * m_texsize.x;
	pixely = texv * m_texsize.y ;

}

//參數：pixel座標(in)，view座標(out)
//說明：從texture的pixel座標，轉成view的pixel座標
bool CMapControl::TexTransformView( float pixelx , float pixely , float &viewx , float &viewy )
{
	float tempx  ;
	float tempy ;
	float viewtexwidth ;
	float viewtexheight ;


	//計算視窗的Texture範圍的寬與高
	viewtexwidth = m_viewsizefortexrightbottom.x - m_viewsizefortexlefttop.x ;
	viewtexheight = m_viewsizefortexrightbottom.y - m_viewsizefortexlefttop.y ;
	//計算左上點的Texture's座標，所對應到Texture的pixel座標
	tempx = m_viewposfortexlefttop.x * m_texsize.x ;
	tempy = m_viewposfortexlefttop.y * m_texsize.y ;
	//求出相差的pixel量
	viewx = pixelx - tempx ; 
	viewy = pixely - tempy ; 

	//預防0
	if ( viewtexwidth < EPSINON )
	{
		viewtexwidth = EPSINON ;
	}
	if ( viewtexheight < EPSINON )
	{
		viewtexheight = EPSINON ;
	}


	//利用view的大小 / 視窗的Texture範圍的寬與高 得來的比例。
	//用此比例來對應到view的正確位置
	viewx = viewx * (m_viewsize.x / viewtexwidth);
	viewy = viewy * (m_viewsize.y / viewtexheight);


	float cosAngle = cos(DegreesToRads(CTextureCoordControl::GetRotation())); 
	float sinAngle = sin(DegreesToRads(CTextureCoordControl::GetRotation())); 

	viewx -= m_viewsize.x * 0.5f ; 
	viewy -= m_viewsize.y * 0.5f  ; 


	float tempX = viewx ; 
	float tempY = viewy  ; 

	//左上
	viewx = (tempX * cosAngle - tempY * sinAngle) ; 
	viewy = (tempX * sinAngle + tempY * cosAngle) ; 

	viewx += m_viewsize.x * 0.5f ; 
	viewy += m_viewsize.y * 0.5f  ; 

	
	if((viewx < 0.0f || viewx > m_viewsize.x) ||
	   (viewy < 0.0f || viewy > m_viewsize.y))
	{
		return false ; 
	}

	return true ;
	
}

//說明：設定目前玩家在小地圖的座標
void CMapControl::SetNavigationPos(float worldx, float worldz)
{
	
	//計算玩家的貼圖座標和Texture的pixel座標
	Transform( worldx , worldz , m_roleposfortex.x , m_roleposfortex.y , 
				m_rolefortex.x , m_rolefortex.y ) ;

	assert( m_pmap != NULL ) ; 
	
	//設定小地圖的貼圖座標
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;

	//再利用Texture的pixel座標轉換到view座標
	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
		m_roleposforview.x , m_roleposforview.y ) ;

}

//參數：人物（物件）的世界座標(in)，此物件的視窗座標(out)
//說明：計算地圖座標以及視窗座標 , 若得到的位置有在視窗範圍內, 回傳true; 反之回傳false
bool CMapControl::GetWindowPos(float wx, float wz, int &x, int &y) 
{
	float texu = -1.0f;
	float texv = -1.0f;
	float screenx = -1.0f;
	float screeny = -1.0f;
	float texx = -1.0f ;
	float texy = -1.0f ;

	//Y軸座標轉換(往下遞減，往上遞增。跟螢幕座標相反)
	wz = m_worldsize.y - wz ;

	//限制人物在世界座標之內
	LimitroleInWorld( wx , wz) ;
 
	//轉換座標            Textrure貼圖座標 ， Texture上的pixel座標
	Transform( wx , wz ,  texu , texv       , texx , texy ) ;


	//float cosAngle = cos(DegreesToRads(45)); 
	//float sinAngle = sin(DegreesToRads(45)); 

	//texu -=  0.5f ; 
	//texv -=  0.5f  ; 


	//float tempX = texu ; 
	//float tempY = texv  ; 

	////左上
	//texu = (tempX * cosAngle - tempY * sinAngle) ; 
	//texv = (tempX * sinAngle + tempY * cosAngle) ; 

	//texu += 0.5f ; 
	//texv += 0.5f  ; 


	//檢查人物（物件）是否在顯示視窗範圍。
	if ( IsInside( texu , texv ) )
	{
		bool isViewed = false ; 

		//將人物的tex's pixel座標轉換到螢幕座標
		isViewed = TexTransformView( texx , texy , screenx , screeny ) ;



		//累加視窗的基準座標
		x = static_cast<int>(screenx)  ;
		y = static_cast<int>(screeny) ;

		return isViewed ;
	}

	x = -1 ;
	y = -1 ;

	return false ;
}

bool CMapControl::GetWorldPos(float screenX , float screenY , float screenWidth , float screenHieght ,
							  float &outWorldX , float &outWorldY )
{

	float scaleX = screenX  / screenWidth ; 
	float scaleY = screenY  / screenHieght ;

	float u = 0.0f ; 
	float v = 0.0f ; 

	float centerX ; 
	float centerY ; 

	if(!m_pmap->GetRotUVCoord(scaleX , scaleY , u , v , centerX , centerY))
	{
		return false ; 
	}

	centerX *= m_worldsize.x ; 
	centerY *= m_worldsize.y ; 

	centerY = m_worldsize.y - centerY ; 


	outWorldX  = u  * m_worldsize.x; 
	outWorldY = m_worldsize.y -(v * m_worldsize.y); 


//	outWorldX += 0.5f ; 
//	outWorldY += 0.5f ; 


	
	return true ; 
}


bool CMapControl::IsMaxZoomIn( void  ) const 
{
	assert( m_pmap != NULL ) ;


	return m_pmap->IsMaxZoomIn( ) ;

	
}

bool CMapControl::IsMinZoomOut( void ) const
{
	assert( m_pmap != NULL ) ;


	return m_pmap->IsMinZoomOut( ) ;

}

//說明：輸入貼圖座標，檢查此座標是否在顯示視窗範圍之內。內的話回傳true，外的話回傳false
bool CMapControl::IsInside(float u, float v)
{
	assert( m_pmap != NULL ) ;

	return m_pmap->IsInsideForShowUV( u , v ) ;

}

//說明：限制人物在世界座標起點 到 世界座標的總寬及高的範圍之內
void CMapControl::LimitroleInWorld( float &x , float &z )
{
	//小於 世界起點
	if ( x <= m_wroldforstart.x )
	{
		x = m_wroldforstart.x ;

	}
	//大於 世界末端
	else if ( x >= m_wroldforstart.x + m_worldsize.x )
	{
		x = m_wroldforstart.x + m_worldsize.x;
	}
	//小於 世界起點
	if (z <= m_wroldforstart.y )
	{
		z = m_wroldforstart.x ;
	}
	//大於 世界末端
	else if ( z >= m_wroldforstart.y + m_worldsize.y )
	{
		z = m_wroldforstart.y + m_worldsize.y;
	}
}


//說明：設定玩家在世界的座標
void CMapControl::SetRoleWorldPos( float x , float z ) 
{
	float offsetx ;
	float offsety ;//移動偏移量

	//Y軸座標轉換(往下遞減，往上遞增。跟螢幕座標相反)
	z = m_worldsize.y - z ;


	//限制玩家移動之後。在世界地圖範圍之內
	LimitroleInWorld( x , z ) ;

	//計算偏移量
	offsetx = x -  m_roleworldpos.x ;
	offsety = z -  m_roleworldpos.y ;


	AddRoleWorldPos( offsetx , offsety ) ;

	m_roleworldpos.x = x ;
	m_roleworldpos.y = z ;
	
	//將人物的世界座標轉成螢幕座標
	SetNavigationPos( m_roleworldpos.x , m_roleworldpos.y ) ;
}

//說明：偏移人物在螢幕座標，還有像機方面的調整
void CMapControl::AddRoleWorldPos( float offsetx , float offsety )
{
	float temprolex ; //暫存人物的位置(texture座標)
	float temproley ; 
	float moveoffsetx ;		//移動偏移量
	float moveoffsety ;

	temprolex = m_roleposfortex.x ;
	temproley = m_roleposfortex.y ;

	m_roleworldpos.x += offsetx ;
	m_roleworldpos.y += offsety ; 


	//計算玩家的貼圖座標和Texture的pixel座標
	Transform( m_roleworldpos.x , m_roleworldpos.y , m_roleposfortex.x , m_roleposfortex.y , 
				m_rolefortex.x , m_rolefortex.y ) ;


	//計算偏移量：移動前的Texture's coord - 移動後的Texture's coord 
	moveoffsetx =  m_roleposfortex.x - temprolex;
	moveoffsety =  m_roleposfortex.y - temproley;


	///////////////////					相機位置修正			//////////////////////////
	//多累加相機位置。累加的量：用來重新調整相機的偏移量。以達到人跟相機一致
	moveoffsetx += temprolex - m_viewposfortex.x ;
	moveoffsety += temproley - m_viewposfortex.y;


	//偏移camera位置
	AddUVTranslate( moveoffsetx , moveoffsety ) ;

}

