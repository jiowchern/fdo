#include "stdafx.h"
#include "mapcontrol.h"
#include "assert.h"
#include "R_BaseType.h"


#define DegreesToRads( degrees ) ((degrees) * (PI/ 180.0f))
#define RadsToDegrees( angle ) ((18.0f) * (angle / PI))

////						�R�A�ܼƫŧi				//////////

float CTextureCoordControl::s_texcoordcenter = 0.5f ; 
float CTextureCoordControl::s_texcoordrightbottom = s_texcoordcenter * 2.0f; 
float CTextureCoordControl::s_worldviewrect = 150.0f ;
float CTextureCoordControl::s_rotation = 45.0f ; 

////
//		���O�GTexture�y�б���
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

	//�w�]��(64,64)
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

//�����G���o�{��UV�y��
void CTextureCoordControl::GetUVpos( float &u , float &v ) const 
{
	u = m_texUVpos.x ;
	v = m_texUVpos.y ; 
}

//�����G���o�{��UV�|�I�y��
void CTextureCoordControl::GetUVposRect( float &left , float &top ,
										float &right , float &bottom ) const 
{
	left = m_texUVleft ;
	top = m_texUVtop ; 
	right = m_texUVright ;
	bottom = m_texUVbottom ;
}

//�����G���o�Y��Y��
void CTextureCoordControl::GetZoom( float &scalex , float &scaley ) const 
{
	scalex = m_texzoom.x  ;
	scaley = m_texzoom.y ;
}

//�����G���o��Texture������e
void CTextureCoordControl::GetTextureSize(float &texwidth, float &texheight) const
{
	texwidth = m_texSize.x ; 
	texheight = m_texSize.y ;
}

//�����G�������VTexture�귽����			add 08/1/21
void CTextureCoordControl::SetTexture(LPDIRECT3DTEXTURE9 tex)
{
	assert( tex != NULL ) ;

	//�]�w����
	m_bshare = true ;

	m_texture = tex ;

	//��sTexture
	UpdateTextureAtt( ) ;


}

//�����G�������VAlphaTexture�귽����		add 08/1/21
void CTextureCoordControl::SetAlphaTexture(LPDIRECT3DTEXTURE9 tex)
{
	assert( tex != NULL ) ;
	
	//�]�w����
	m_bshare = true ;

	m_alphatex = tex ;
}

//�����GŪ���Ϥ�form disk
void CTextureCoordControl::LoadTexture(LPCWSTR filename)
{
	m_bshare = false ;

	D3DXCreateTextureFromFile( g_pD3DDevice , filename , &m_texture ) ;

	//��sTexture
	UpdateTextureAtt( ) ;
}

//�����GŪ��alpha��
void CTextureCoordControl::LoadAlphaTexture(LPCWSTR filename)
{
	m_bshare = false ;

	D3DXCreateTextureFromFile( g_pD3DDevice , filename , &m_alphatex ) ;

	assert( m_alphatex != NULL ) ;

}

//�����G��sTexture�ݩ�				add 08/1/21
void CTextureCoordControl::UpdateTextureAtt( void )
{
	assert( m_texture != NULL ) ;
	if(m_texture == 0)
		return;

	////////////			�]�w�����ݩ�		////////////////////
	D3DSURFACE_DESC desc ;

	m_texture->GetLevelDesc( 0 , &desc ) ;


	//�]�wver's Size
	float tempx = static_cast<float>(desc.Width) ;
	float tempy = static_cast<float>(desc.Height) ;

/*
	//�p��̤j����j�Y��
	if ( tempx > tempy )
	{
		//��e�פj�󰪫׮ɡA�p���ҡC�Ѧ���Ҩӭ���U�y��
		//�|���Ws_texcoordcenter����]�O�Am_texminzoom�O�����O�`U���@�b
		m_texminzoom.x = (tempy / tempx) * s_texcoordcenter ;
		m_texminzoom.y = s_texcoordcenter ;
		//������Texture�������I
		m_texUVcenter.x = s_texcoordcenter + m_texminzoom.x ;
	}
	else if ( tempy < tempx )
	{
		m_texminzoom.y = (tempx / tempy) * s_texcoordcenter ;
		m_texminzoom.x = s_texcoordcenter ;
		//������Texture�������I
		m_texUVcenter.y = s_texcoordcenter + m_texminzoom.y ;
	}*/

/*
	//�]�wver's Size
	m_versize.x = static_cast<float>(desc.Width) ;
	m_versize.y = static_cast<float>(desc.Height) ;

	//�p��̤j����j�Y��
	if ( m_versize.x > m_versize.y )
	{
		//��e�פj�󰪫׮ɡA�p���ҡC�Ѧ���Ҩӭ���U�y��
		//�|���Ws_texcoordcenter����]�O�Am_texminzoom�O�����O�`U���@�b
		m_texminzoom.x = (m_versize.y / m_versize.x) * s_texcoordcenter ;
		m_texminzoom.y = s_texcoordcenter ;
		//������Texture�������I
		m_texUVcenter.x = s_texcoordcenter + m_texminzoom.x ;
	}
	else if ( m_versize.y < m_versize.x )
	{
		m_texminzoom.y = (m_versize.x / m_versize.y) * s_texcoordcenter ;
		m_texminzoom.x = s_texcoordcenter ;
		//������Texture�������I
		m_texUVcenter.y = s_texcoordcenter + m_texminzoom.y ;
	}
*/
	
	//�]�wUV�e�פj�p(�Y���̤j)
	SetTexUVSize( m_texmaxzoom , m_texmaxzoom ) ;

	//�]�wtex's Size
	m_texSize.x = tempx ;
	m_texSize.y = tempy ;

	
	//���s�]�w���I����m
//	SetVertexSize(  static_cast<int>(m_versize.x) , static_cast<int>(m_versize.y) ) ;
}

//�����G�O�_�Y���̤j�]���Y�Ԩ�̻��^
bool CTextureCoordControl::IsMaxZoomIn( void ) const 
{
	if ( m_texUV.x >= m_texminzoom.x && m_texUV.y  >= m_texminzoom.y )
	{
		return true ;
	}

	return false ;	
}

//�����G�O�_�Y���̤p�]���Y�Ԩ�̪�^
bool CTextureCoordControl::IsMinZoomOut( void ) const
{
	if ( m_texUV.x <= m_texmaxzoom  && m_texUV.y  <= m_texmaxzoom)
	{
		return true ;
	}

	return false ;
}


//���R�G���o���I���e�P��
void CTextureCoordControl::GetVertexSize(float &width, float &height) const 
{
	width = m_versize.x ; 
	height = m_versize.y ;
}

//�����G���o���I��m�]���W�I�^
void CTextureCoordControl::GetVertexLeftTop( float &left , float &top ) const 
{
	left = m_verpos.x - m_versize.x / 2 ; 
	top  = m_verpos.y - m_versize.y / 2; 
}

//�����G�]�w���I��m�]���W�I�^
void CTextureCoordControl::SetVertexPos(int posx, int posy) 
{
	//����
	m_verpos.x = posx + (m_versize.x * 0.5f)   ;
	m_verpos.y = posy + (m_versize.y * 0.5f) ;

	//��s���I��m
	UpdateVertex( ) ;
}

//�����G�]�w��Texture�y�Ъ��Y�p�Y��
void CTextureCoordControl::SetMaxZoomOut( float value )
{
	if ( value >= s_texcoordcenter )
	{
		value = s_texcoordcenter ; 
	}
	m_texmaxzoom = value ;
}

//�����G�]�w��Texture�y�Ъ���j�Y��   
void CTextureCoordControl::SetMaxZoomIn( float worldwidth , float worldheight)
{
	m_texminzoom.x = (s_worldviewrect / worldwidth) * s_texcoordcenter ;
	m_texminzoom.y = (s_worldviewrect / worldheight) * s_texcoordcenter ;
}

//�����G�]�w�Y�񪺸ѪR��
void CTextureCoordControl::ResetZoomValue( float value , float time )
{
	float zoomwidth ;
	float zoomheight ;


	//�p���Y�񪺶Z��
	zoomwidth = m_texminzoom.x - m_texmaxzoom  ;
	zoomheight = m_texminzoom.y - m_texmaxzoom ;


	//�]���Z���۵��Avalue�H�۹q�����t�Ƥ��P�ӧ��ܡAtime�O������q�C
	//�p�G20m(zoomwidth)�A��@����(value)�]���Ctime�O����Ҫ᪺�ɶ��C
	float offset = value / time ;


	//�����x�������
	m_texzoom.x = offset * zoomwidth   ; 
	m_texzoom.y = offset * zoomheight  ; 





}

//�����G�]�w�Y�񪺭���
void CTextureCoordControl::SetZoomMultiple( float multiple )
{
	//�o�̬O�ϥΪ̪����]�w�ѪR�סC���ݭn��perframetime�ӨM�w�C
	//�o�̾A�X�ƹ��u���h�ϥΡC��p�Gmultiple��6���ܡA�u�n�u��
	//��6���N��F�����F�C
	m_texzoommultiple = multiple ;

	//�u�n�Y�񭿼Ƨ�s�ɡA�N�n���s�Y��Y��
	m_texzoom.x = (m_texminzoom.x - m_texmaxzoom) / multiple ;
	m_texzoom.y = (m_texminzoom.y - m_texmaxzoom) / multiple ;
}


//�����G�]�w���I���e�򰪫�
void CTextureCoordControl::SetVertexSize(int sx, int sy)
{

	D3DXVECTOR2 half ;

	//��s���Isize
	m_versize.x = static_cast<float>(sx) ;
	m_versize.y = static_cast<float>(sy) ;

	half.x = static_cast<float>(sx) ;//�e
	half.y = static_cast<float>(sy) ;//��

	D3DXVec2Scale( &half , &half , 0.5f ) ;

	//�o�O�ù��y�Шt

	//���W
	m_verbasepos[0].x = -half.x ;
	m_verbasepos[0].y = -half.y ;
	//���U
	m_verbasepos[1].x = -half.x ;
	m_verbasepos[1].y = half.y ;
	//�k�U
	m_verbasepos[2].x = half.x ;
	m_verbasepos[2].y = half.y ;
	//�k�W
	m_verbasepos[3].x = half.x ;
	m_verbasepos[3].y = -half.y ;

	//��s���I��m
	UpdateVertex( ) ;

}

//�����G�֥[UV�y�Ц�m
void CTextureCoordControl::AddUVTranslate(float offsetx, float offsety)
{

	//UV�������I
	float u = m_texUVpos.x ;
	float v = m_texUVpos.y ;

	//�H�����I����ǡA��sTexRect�y��
	UpdateUVCoord( u , v , m_texUV.x , m_texUV.y ) ;

	float tempu = -1.0f ;
	float tempv = -1.0f ;

	//�ˬdRECT�d��C(tempu , tempv)�N�O�W�L 0.0 <= value <= 1.0f�������q
	//�M��h�ץ��]��X�Ӫ�tempu , tempv�^
	if ( CheckInside( m_texUVleft , m_texUVtop , m_texUVright , m_texUVbottom , 0.0f  , s_texcoordrightbottom  , tempu , tempv ) )
	{
		//////////////		�|�W�L���S�O��]�G�]���Y�pTexture�ɡA�M�Ჾ�ʨ�x�����Y�@�ө��u�A
		/////////////						  �A���Y�pTexture�ɡA�b���ʨ�x��������V�����u�A
		////////////						  �M��b��jTexture�A�|�y���W�X 0.0 <= value <= s_texcoordrightbottom�d��A
		///////////							  �o�OUpdateZoomCenter�����Y�C�b�o��@�ӭץ��C

		//�p�G�����ܪ��ܡA�i��ץ�
		if ( ( tempu + 1.0f ) >= 0.0001f )
		{
			//�P�_�@���@��V���ץ�
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
			//�P�_�@���@��V���ץ�
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
		//			�Hu,v�������I�A�h����m_texUV�A�h�M�wm_texUVRect���d��C			////////
		//			�H�U�O�P�_�O�_�٦��h�l���Ŷ��A�h�@�����C						///////

		//�p������ΤW�䰾����
		if ( offsetx < 0.0f || offsety < 0.0f  )
		{
			//�P�_u�O�_�٦��h�lm_texUV���Ŷ��h�����C�p�G�����ܡA�N����
			if ( u > 0.0f && u - m_texUV.x > 0.0f )
			{
				//��sUV�y��
				m_texUVpos.x += offsetx ;
			}
			if ( v > 0.0f && v - m_texUV.y  > 0.0f )
			{
				m_texUVpos.y += offsety ;
			}
		}
		//�p����k�ΤU�䰾����
		if ( offsetx > 0.0f || offsety > 0.0f )
		{
			//�P�_u�O�_�٦��h�lu + m_texUV���Ŷ��h�����C�p�G�����ܡA�N����
			if ( u > 0.0f && u + m_texUV.x < s_texcoordrightbottom )
			{
				//��sUV�y��
				m_texUVpos.x += offsetx ;
			}
			if ( v > 0.0f && v + m_texUV.y  < s_texcoordrightbottom )
			{
				m_texUVpos.y += offsety ;
			}
		}
		//���F�A���ܤ��ߦ�m���~�A�٭n����UV's Rect�y��
		UpdateUVCoord( m_texUVpos.x , m_texUVpos.y , m_texUV.x , m_texUV.y ) ;
	}

	//��stexUV��m
	UpdateVertex( ) ;

}

//�п�Jvalue <= 0.5f �C
//�����G�]�w�K�Ϯy�мe�P���C���]�`�e�׬�1.0�C���n�]�w��u's size��0.5�A���פ]�@�ˡC�Ъ`�N�I
void CTextureCoordControl::SetTexUVSize(float u, float v)
{
	m_texUV.x = u ;
	m_texUV.y = v ;

	//��s���I�����ݩ�
	UpdateVertex( ) ;
}

//�����G�]�w�K�Ϯy�Ц�m
void CTextureCoordControl::SetTexUVPos( float u , float v )
{
	//�H�����I����ǡA�ȦsTexRect�y��

	UpdateUVCoord( u , v , m_texUV.x , m_texUV.y ) ;

	m_texUVpos.x = u ;
	m_texUVpos.y = v ;

	
	//��s���I�����ݩ�
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


//�����G�]�wUV�y�Ц�m
void CTextureCoordControl::SetUVTranslate(float x, float y)
{
	m_texUVpos.x = x ;
	m_texUVpos.y = y ;

	//��stexUV��m
	UpdateVertex( ) ;

}


//�����G��s���I�����ݩ�
void CTextureCoordControl::UpdateVertex( void )
{	

	///////////////////				�]�w���I��m			///////////////////
	//���W
	float vx0 = m_verpos.x + m_verbasepos[0].x ;
	float vy0 = m_verpos.y + m_verbasepos[0].y ;
	//���U
	float vx1 = m_verpos.x + m_verbasepos[1].x  ;
	float vy1 = m_verpos.y + m_verbasepos[1].y  ;
	//�k�U
	float vx2 = m_verpos.x + m_verbasepos[2].x  ;
	float vy2 = m_verpos.y + m_verbasepos[2].y ;
	//�k�W
	float vx3 = m_verpos.x + m_verbasepos[3].x  ;
	float vy3 = m_verpos.y + m_verbasepos[3].y ;


	//UV�������I
	float u = m_texUVpos.x ;
	float v = m_texUVpos.y ;

	//�åB��s�Y�񤤤��I�C���Om_texUVpos���ܳ�I
	UpdateZoomCenter( u , v ) ;

	m_cameraUV.x = u ; 
	m_cameraUV.y = v ; 


	//��sUV���|�I�y��
	UpdateUVCoord( u , v , m_texUV.x , m_texUV.y ) ;




	float newUVLeftTopX , newUVLeftTopY , newUVLeftBottomX , newUVLeftBottomY ; 
	float newUVRightBottomX , newUVRightBottomY , newUVRightTopX , newUVRightTopY ; 

	m_texUVleft -= u ;  
	m_texUVtop -= v ; 
	m_texUVright -= u ;
	m_texUVbottom -=v ; 

	float cosAngle = cos(DegreesToRads(-CTextureCoordControl::GetRotation())); 
	float sinAngle = sin(DegreesToRads(-CTextureCoordControl::GetRotation())); 


	//���W
	newUVLeftTopX = u + (m_texUVleft * cosAngle - m_texUVtop * sinAngle) ; 
	newUVLeftTopY = v + (m_texUVleft * sinAngle + m_texUVtop * cosAngle) ; 

	//���U
	newUVLeftBottomX = u + (m_texUVleft * cosAngle - m_texUVbottom * sinAngle) ; 
	newUVLeftBottomY = v + (m_texUVleft * sinAngle + m_texUVbottom * cosAngle) ; 

	//�k�U
	newUVRightBottomX = u + (m_texUVright * cosAngle - m_texUVbottom * sinAngle) ; 
	newUVRightBottomY = v + (m_texUVright * sinAngle + m_texUVbottom * cosAngle) ; 


	//�k�W
	newUVRightTopX = u + (m_texUVright * cosAngle - m_texUVtop * sinAngle) ; 
	newUVRightTopY = v + (m_texUVright * sinAngle + m_texUVtop * cosAngle) ; 

	//��sUV���|�I�y��
	m_texUVleft +=  u;
	m_texUVtop += v;
	m_texUVright +=  u;
	m_texUVbottom +=  v ;



	////////////////			�]�wUV��m			/////////////////////////
	//SetVertex( &m_mapvertex[0] , vx0 , vy0 , 0.0f , 1.0f , m_texUVleft  , m_texUVtop , 0.0f , 0.0f) ;
	//SetVertex( &m_mapvertex[1] , vx1 , vy1 , 0.0f , 1.0f , m_texUVleft  , m_texUVbottom , 0.0f , 1.0f ) ;
	//SetVertex( &m_mapvertex[2] , vx2 , vy2 , 0.0f , 1.0f , m_texUVright  ,m_texUVbottom , 1.0f , 1.0f  ) ;
	//SetVertex( &m_mapvertex[3] , vx3 , vy3 , 0.0f , 1.0f , m_texUVright , m_texUVtop , 1.0f , 0.0f ) ;

	SetVertex( &m_mapvertex[0] , vx0 , vy0 , 0.0f , 1.0f , newUVLeftTopX  , newUVLeftTopY , 0.0f , 0.0f) ;
	SetVertex( &m_mapvertex[1] , vx1 , vy1 , 0.0f , 1.0f , newUVLeftBottomX  , newUVLeftBottomY , 0.0f , 1.0f ) ;
	SetVertex( &m_mapvertex[2] , vx2 , vy2 , 0.0f , 1.0f , newUVRightBottomX  ,newUVRightBottomY , 1.0f , 1.0f  ) ;
	SetVertex( &m_mapvertex[3] , vx3 , vy3 , 0.0f , 1.0f , newUVRightTopX , newUVRightTopY , 1.0f , 0.0f ) ;

}

//�����G��s�Y�񤤤��I
void CTextureCoordControl::UpdateZoomCenter(float &u, float &v)
{

	////				�H(0.5f , 0.5f)�������I		//////////////////

	//�D�n�[���G�Y�񤤤ߥ����H�ɪ`�N��A�P�򪺥|�Ӯy�Ц�m�A�O��m_texUV���e�P�����~�A
	//�٭n�`�N��A���� �p��0 �� �j��1.0f ���K�Ϯy�нd��C

	//�ҥH�A�Ĥ@��if�P�_���P�_(u ,v)�b�ĴX�����C
	//�̭�����ӧP�_���G�� u < 0.5f �����p�U�Au <= m_texUV.x �ɡA��ܤ���O��m_texUV���e�A�ҥH�����a�Jm_texUV���e
	//					�� u > 0.5f �����p�U�Au + m_texUV.x > 1.0f �A�N�O�n�P�_�O�_�W�L�K�Ϯy�Ъ��̤j�ȡA
	//					�p�G�W�L���ܡA�N���ߪ�u �������ȡA�N���|�W�L1.0f�F�C���A�Ѫ��ܡA�еe�ϧa�I

	//�̫�A�u�n��X�䤤�@�ӶH���N��F 

	//���O�{�b�U�����{�ǡA�����I���O�T�w�b(0.5f , 0.5f)�F


	//��2����
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

	//��1����
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

	//��3����
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

	//��4����
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


	//�k
	if ( u > s_texcoordcenter && ((v - s_texcoordcenter) < EPSINON) )
	{
		float tempU = u + m_texUV.x ;

		if ( tempU >= s_texcoordrightbottom)
		{
			u -= (tempU - s_texcoordrightbottom) ;
		}

		return ;
	}

	//�W
	if ( ((u - s_texcoordcenter) < EPSINON)  && v < s_texcoordcenter)
	{
		if ( v <= m_texUV.y )
		{
		   v = m_texUV.y ;
		}

		return ;
	}

	//��
	if ( u < s_texcoordcenter && ((v - s_texcoordcenter) < EPSINON) )
	{
		if ( u <= m_texUV.x )
		{
			u = m_texUV.x ;
		}

		return ;
	}

	//�U
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

//�����G��s�K�Ϯy�Ъ��|���I
void CTextureCoordControl::UpdateUVCoord( float centeru , float centerv, float uwidth , float vheight )
{
	//��sUV���|�I�y��
	m_texUVleft = centeru - uwidth ;
	m_texUVtop = centerv - vheight ;
	m_texUVright = centeru + uwidth ;
	m_texUVbottom =  centerv + vheight ;

}

//�ѼơG��1.2�Ѽƽd��0~1
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

//�����G�ˬdRECT���y�СA�O�_���W�Lmin�Mmax������d��C�p�G�����ܡA�^��true
bool CTextureCoordControl::CheckInside( float &left , float &top , float &right , float &bottom , float min , 
									float max , float &u , float &v)
{
	bool chang = false ;

	//����UV�y�Э���
	if ( left < min )
	{
		u = -left ;

		left = min ;

		chang = true ;
	}
	//�k��UV�y�Э���
	if ( right > max )
	{
		u = right - max ;

		right = max ;

		chang = true ;
	}
	//�W��UV�y�Э���
	if ( top < min )
	{
		v = -top ;

		top = min ;

		chang = true ;
	}
	//�U��UV�y�Э���
	if ( bottom > max )
	{
		v = bottom - max ;

		bottom = max ; 

		chang = true ;
	}
	
	return chang ;
}

//�����G�P�_���K�Ϯy�ЬO�_�b��ܶK�ϥ|�y�Ъ��d�򤧤�
bool CTextureCoordControl::IsInsideForShowUV(float u, float v)
{

	//���I
	if ( u >= (m_texUVleft ) &&
		u < (m_texUVright )  &&
		v >= (m_texUVtop)  &&
		v <= (m_texUVbottom ) )
	{
		return true ;
	}

   //�~�I
   return false;
	
}




//�����G�]�wVERTEX2D�����ݩ�
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
	case WM_LBUTTONUP : //��}����A�]�w���I��m
		{
			int curX = LOWORD(lParam) ;
			int curY = HIWORD(lParam) ;

			SetVertexPos( static_cast<float>(curX) , static_cast<float>(curY) ) ;
		}break ;
	case WM_MOUSEWHEEL: //�ƹ��u���T��
		{
			int delta = static_cast<int>(wParam) ;

			//�V�W�u�ʮ�
			if ( delta > 0 )
			{
				//�Y�p
				TexZoomIn( ) ;
			}
			//�V�U�u�ʮ�
			else if ( delta < 0 ) 
			{
				//��j
				TexZoomOut( ) ;
			}
		}break ;*/
	/////////////////	��L	/////////////
 /*   case WM_KEYUP:
        {
            switch( wParam )
            {
				case VK_LEFT:
				{
					//�n�`�N�������q����ǫ�
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
				case 107 :  // "+"��
				{
					TexZoomOutMax( ) ;
					return true ;
				}
				case 109 : // "-"��
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



//�����G�Y�pTexture's uv
void CTextureCoordControl::TexZoomIn( void )
{
	//�]�w�Y�p���ܡA�N�仡�G�[�jUV���y��
	m_texUV.x += m_texzoom.x ;
	m_texUV.y += m_texzoom.y ;

	//����UV�y��
	if ( m_texUV.x >= m_texminzoom.x )
	{
		m_texUV.x = m_texminzoom.x ;
	}
	if ( m_texUV.y >= m_texminzoom.y)
	{
		m_texUV.y = m_texminzoom.y ;
	}

	//��sUV�y��
	UpdateVertex( ) ;
}

//�����G��jTexture' uv 
void CTextureCoordControl::TexZoomOut( void )
{
	//�]�w��j���ܡA�N�仡�G���UV���y��
	m_texUV.x -= m_texzoom.x ;
	m_texUV.y -= m_texzoom.y ;


	//����UV�y��
	if ( m_texUV.x <= m_texmaxzoom )
	{
		m_texUV.x = m_texmaxzoom ;
	}
	if ( m_texUV.y <= m_texmaxzoom )
	{
		m_texUV.y = m_texmaxzoom ;
	}

	//��sUV�y��
	UpdateVertex( ) ;
}

//�����G�Y�p��̤j����
void CTextureCoordControl::TexZoomInMax( void )
{	
	m_texUV.x = m_texminzoom.x ;
	m_texUV.y = m_texminzoom.y ;

	//��sUV�y��
	UpdateVertex( ) ;		
}

//�����G��j��̤j����
void CTextureCoordControl::TexZoomOutMax( void )
{
	m_texUV.x = m_texmaxzoom ;
	m_texUV.y = m_texmaxzoom ;

	//��sUV�y��
	UpdateVertex( ) ;		
	
}

//�����G�ϥ�aplhaTex
void CTextureCoordControl::DrawTexWithAlphaTex(IDirect3DDevice9 *d3d)
{

	assert( m_alphatex != NULL ) ;
	assert( m_texture != NULL ) ;

	//���}alpha�q�D
	d3d->SetRenderState( D3DRS_ALPHABLENDENABLE , true ) ;

	//render
	d3d->SetFVF(MY_FVF_VERTEX);
	
	d3d->SetTexture( 0 , m_texture ) ;
	d3d->SetTexture( 1 , m_alphatex ) ;


	//�Ntexture and diffuse �@alpha�V��  
	d3d->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_MODULATE    ) ;
	d3d->SetTextureStageState( 0 , D3DTSS_ALPHAARG1  , D3DTA_TEXTURE ) ;
	d3d->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_DIFFUSE ) ;

	//�Ntexture and diffuse �@color�V��
	d3d->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_MODULATE   ) ;
	d3d->SetTextureStageState( 0 , D3DTSS_COLORARG1 , D3DTA_TEXTURE ) ;
	d3d->SetTextureStageState( 0 , D3DTSS_COLORARG2 , D3DTA_DIFFUSE ) ;

	//�@alpha�V��  
	d3d->SetTextureStageState( 1 , D3DTSS_ALPHAARG1  , D3DTA_TEXTURE ) ;
	d3d->SetTextureStageState( 1 , D3DTSS_ALPHAARG2 , D3DTA_CURRENT ) ;
	d3d->SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_MODULATE   ) ;

	//�@color�V��
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

//�����G���ϥ�aplhaTex
void CTextureCoordControl::DrawTex(IDirect3DDevice9 *d3d)
{
	assert( m_texture != NULL ) ;

	//render
	d3d->SetFVF(MY_FVF_VERTEX);

	//�Ntexture and diffuse �@alpha�V��  
	//d3d->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG1    ) ;
	//d3d->SetTextureStageState( 0 , D3DTSS_ALPHAARG1  , D3DTA_TEXTURE ) ;
	//d3d->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_DIFFUSE ) ;

	////�Ntexture and diffuse �@color�V��
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
//		���O�G�p�a�ϱ���
//		�����G�~�Ӧ�CDXUTControl
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

	//��s�����ݩ�
	UpdateView( ) ;
}

void CMapControl::SetWindowPos( int posx , int posy )
{
	assert( m_pmap != NULL ) ;

	//�����o���I��m(����VerSize���w�g�]�w����)
	m_pmap->SetVertexPos( posx , posy ) ;

	//��s�����ݩ�
	UpdateView( ) ;

}

//�����G���o�H���b�ù��y�Ц�m
void CMapControl::GetRoleViewcoord(int &posx, int &posy) const
{
	
	posx = static_cast<int>(m_roleposforview.x)  ;
	posy = static_cast<int>(m_roleposforview.y)  ;
}


void CMapControl::SetWindowSize( int sx , int sy )
{
	assert( m_pmap != NULL ) ;

	m_pmap->SetVertexSize( sx , sy ) ;

	//��s�����ݩ�
	UpdateView( ) ;

}

//�����G�]�w�Y�񶡮�
void CMapControl::SetZoomParam( float zoomtime )
{
	//�o�ƭȪ�ܡG�q�@���Y���I��t�@���Y���I�A�Ҫ�O���ɶ��C
	m_zoomtime = zoomtime ;

}

void CMapControl::SetMaxZoomOut(float value)
{
	assert( m_pmap != NULL ) ;

	m_pmap->SetMaxZoomOut( value ) ;
}

//�����G��j
void CMapControl::TexZoomIn( void )
{
	assert( m_pmap != NULL ) ;

	//�Y�񤧫e�A�]�wUV�y�Ь��H����Texture��m
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;

	//�A�i���j
	m_pmap->TexZoomIn( ) ;

	//�b��s�����ݩ�
	UpdateView( ) ;


	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
	m_roleposforview.x , m_roleposforview.y ) ;
}

//�����G���̤j
void CMapControl::TexZoomInMax( void )
{
	assert( m_pmap != NULL ) ;

	//�Y�񤧫e�A�]�wUV�y�Ь��H����Texture��m�C
	//�]���Y��O�H�H������ǡC
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;
	
	m_pmap->TexZoomInMax( ) ;

	//��s�����ݩ�
	UpdateView( ) ;

	//�ഫ�H���y�Ш�����y��
	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
	m_roleposforview.x , m_roleposforview.y ) ;


}

//�����G�Y�p
void CMapControl::TexZoomOut( void )
{
	assert( m_pmap != NULL ) ;

	//�Y�񤧫e�A�]�wUV�y�Ь��H����Texture��m�C
	//�]���Y��O�H�H������ǡC
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;

	m_pmap->TexZoomOut( ) ;

	//��s�����ݩ�
	UpdateView( ) ;

	//�ഫ�H���y�Ш�����y��
	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
	m_roleposforview.x , m_roleposforview.y ) ;
}

//�����G�Y��̤p
void CMapControl::TexZoomOutMax( void )
{
	assert( m_pmap != NULL ) ;

	//�Y�񤧫e�A�]�wUV�y�Ь��H����Texture��m�C
	//�]���Y��O�H�H������ǡC
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;

	m_pmap->TexZoomOutMax( ) ;

	//�ഫ�H���y�Ш�����y��
	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
	m_roleposforview.x , m_roleposforview.y ) ;
}

void CMapControl::SetTexUVPos( float u , float v )
{
	assert( m_pmap != NULL ) ;

	m_pmap->SetTexUVPos( u , v ) ;

	//��s�����ݩ�
	UpdateView( ) ;
}

void CMapControl::SetMapFile(LPCWSTR filename)
{
	assert( m_pmap != NULL ) ;

	// >> add 08/1/21
	CDXUTElement* pElement = NULL ;
	CDXUTDialogResourceManager* pManager = NULL ;
	DXUTTextureNode *pnode = NULL ;

	//Ū��
	CDXUTControl::SetTexture( filename , NULL , 0 ) ;
	//�줸��
	pElement = CDXUTControl::GetElement( 0 ) ;
	
	assert( pElement != NULL ) ;
	//��귽
	pManager = m_pDialog->GetManager() ;

	assert( pManager != NULL ) ;
	
	//��Texture's node
	pnode = pManager->GetTextureNode( pElement->iTexture );

	//��Texture
	m_pmap->SetTexture( pnode->pTexture ) ;
	// << add 08/1/21


	
	//m_pmap->LoadTexture( filename ) ;

	//��s�p�a�Ϥj�p
	m_pmap->GetTextureSize( m_texsize.x , m_texsize.y ) ;

}

void CMapControl::SetMapMaskFile(LPCWSTR filename)
{
	assert( m_pmap != NULL ) ;
	// >> add 08/1/21
	CDXUTElement* pElement = NULL ;
	CDXUTDialogResourceManager* pManager = NULL ;
	DXUTTextureNode *pnode = NULL ;

	//Ū��
	CDXUTControl::SetTexture( filename , NULL , 1 ) ;
	//�줸��
	pElement = CDXUTControl::GetElement( 1 ) ;
	
	assert( pElement != NULL ) ;
	//��귽
	pManager = m_pDialog->GetManager() ;

	assert( pManager != NULL ) ;
	
	//��Texture's node
	pnode = pManager->GetTextureNode( pElement->iTexture );

	//��AlphaTexture
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
	case WM_LBUTTONDOWN : //���U����A�Y��
		{
			//�]�w��j���A�C
			SetZoomState( ZOOMIN ) ;
		}break ;
	case WM_LBUTTONUP : //��}����A�����Y��
		{
			//�]�w��j���A�C
			SetZoomState( ZOOMNULL ) ;
		}break ;
	case WM_RBUTTONDOWN:  //���U�k��A�Y��
		{
			//�]�w�Y�p���A�C
			SetZoomState( ZOOMOUT ) ;
		}break ;
	case WM_RBUTTONUP:  //��}�k��A�����Y��
		{
			//�]�w�Y�p���A�C
			SetZoomState( ZOOMNULL ) ;
		}break ;
	////////////////	��L	/////////////
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
				case 107 :  // "+"��
				{
					TexZoomOutMax( ) ;
					return true ;
				}
				case 109 : // "-"��
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


	//��lGUI��Type
	m_Type = DXUT_CONTROL_MAPCONTROL ;


}

void CMapControl::Release( void )
{
	delete m_pmap ;
	m_pmap = NULL ;


}


//�ѼơG�A�@�ɦa�Ϫ��e�P���A�@�ɦa�Ϫ��_�l�y��(x,z)�A�w�]�Ȭ�(0 , 0)
//�����G
void CMapControl::ResetMapAtt( float worldwidth, float worldheight , float x , float z  )
{
	assert( m_pmap != NULL ) ;

	m_worldsize.x = worldwidth ;
	m_worldsize.y = worldheight ;

	m_wroldforstart.x = x ;
	m_wroldforstart.y = z ;

	//�Q�Υ@�ɪ��e�׻P���סA�ӭp��̤j���Y��Y�� 
	m_pmap->SetMaxZoomIn( worldwidth , worldheight) ;

	//��s���
	UpdateScale( ) ;

}

//�����G�]�w�Y�񪬺A
void CMapControl::SetZoomState( ZoomState state )
{
	m_zoomstate = state ; 
}

//�����G���o�Y�񪺪��A
ZoomState CMapControl::GetZoomState( void ) const 
{
	return m_zoomstate ;
}

//�����G��s�Y��ɶ�
void CMapControl::UpdateZoomTime( float fElapsedTime )
{

	//�O�L�Y�񪬺A����
	if ( m_zoomstate == ZOOMNULL  )
	{
		return ;
	}

	assert( m_pmap != NULL ) ;

	//���s�]�w�Y��Y�ơC
	m_pmap->ResetZoomValue( fElapsedTime , m_zoomtime ) ;
	//�i���Y�񪺰ʧ@
	Zoom( ) ;
	
}

//�����G�i���Y��(���ެO�p�άO�j)
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

//�����G�p��@�ɮy�� �� �p�a�Ϯy�Ъ����
void CMapControl::UpdateScale( void )
{
	float widthscale ;
	float heightscale ;
	
	//�@��size �� �p�a��size �����
	widthscale = m_worldsize.x / m_texsize.x ;
	heightscale = m_worldsize.y / m_texsize.y ;
	//�@�ɪ��y�Ъ�pixel �� �p�a�Ϫ�pixel �����
	m_worldtomapscale.x = (1 / widthscale) / m_texsize.x ;
	m_worldtomapscale.y = (1 / heightscale) / m_texsize.y ;

}

//�����G��s�������j�p�M��m(�̳y�����O)			add 1/21
void CMapControl::UpdateRects( void )
{
	//��s�����O����Rect 
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

	//��s��m
	SetWindowPos(  tempx , tempy  ) ;

}

//�����G��s�����������ݩ�
void CMapControl::UpdateView( void )
{
	assert( m_pmap != NULL ) ;

	//���o���I���W��m
	m_pmap->GetVertexLeftTop( m_viewposforstart.x , m_viewposforstart.y ) ;

	//��s���ISize�ݩ�
	m_pmap->GetVertexSize( m_viewsize.x , m_viewsize.y ) ;

	//�H�ɫO����sviewposfortex
	m_pmap->GetUVpos( m_viewposfortex.x , m_viewposfortex.y ) ;
	//�H�ɫO����sUV��Rect�y��
	m_pmap->GetUVposRect( m_viewposfortexlefttop.x   ,  m_viewposfortexlefttop.y , 
						  m_viewposfortexrightbottom.x , m_viewposfortexrightbottom.y ) ;

	//��s��������Texture���d��j�p�C
	//�p�G(256 , 256)���k�U�������j�p�A�O�bTexture����(1024 , 512)����m
	m_viewsizefortexrightbottom.x = m_viewposfortexrightbottom.x * m_texsize.x ;
	m_viewsizefortexrightbottom.y = m_viewposfortexrightbottom.y * m_texsize.y ;
	//�o�O���W������
	m_viewsizefortexlefttop.x = m_viewposfortexlefttop.x * m_texsize.x ;
	m_viewsizefortexlefttop.y = m_viewposfortexlefttop.y * m_texsize.y ;
}

//�ѼơG�@�ɮy�С]in�^�A�K�Ϯy�С]out�^�ATexture��Pixel�y�С]out�^
//�����G�Q�Υ@�ɮy�СA�D�X�K�Ϯy�ЩM�����y��
void CMapControl::Transform(float worldx, float worldz, float &texu, float &texv,
							float &pixelx, float &pixely)
{
	float rolex ;
	float roley ;

	//�p�⤧��Grole�y�з|�b(0~256)����
	rolex = worldx - m_wroldforstart.x ;
	roley = worldz - m_wroldforstart.y ;

	//�p��H���btexture's coord���d��
	texu =  m_worldtomapscale.x * rolex ;
	texv =  m_worldtomapscale.y * roley ;


	//�O��H���btexture����pixel��m
	pixelx = texu * m_texsize.x;
	pixely = texv * m_texsize.y ;

}

//�ѼơGpixel�y��(in)�Aview�y��(out)
//�����G�qtexture��pixel�y�СA�নview��pixel�y��
bool CMapControl::TexTransformView( float pixelx , float pixely , float &viewx , float &viewy )
{
	float tempx  ;
	float tempy ;
	float viewtexwidth ;
	float viewtexheight ;


	//�p�������Texture�d�򪺼e�P��
	viewtexwidth = m_viewsizefortexrightbottom.x - m_viewsizefortexlefttop.x ;
	viewtexheight = m_viewsizefortexrightbottom.y - m_viewsizefortexlefttop.y ;
	//�p�⥪�W�I��Texture's�y�СA�ҹ�����Texture��pixel�y��
	tempx = m_viewposfortexlefttop.x * m_texsize.x ;
	tempy = m_viewposfortexlefttop.y * m_texsize.y ;
	//�D�X�ۮt��pixel�q
	viewx = pixelx - tempx ; 
	viewy = pixely - tempy ; 

	//�w��0
	if ( viewtexwidth < EPSINON )
	{
		viewtexwidth = EPSINON ;
	}
	if ( viewtexheight < EPSINON )
	{
		viewtexheight = EPSINON ;
	}


	//�Q��view���j�p / ������Texture�d�򪺼e�P�� �o�Ӫ���ҡC
	//�Φ���Ҩӹ�����view�����T��m
	viewx = viewx * (m_viewsize.x / viewtexwidth);
	viewy = viewy * (m_viewsize.y / viewtexheight);


	float cosAngle = cos(DegreesToRads(CTextureCoordControl::GetRotation())); 
	float sinAngle = sin(DegreesToRads(CTextureCoordControl::GetRotation())); 

	viewx -= m_viewsize.x * 0.5f ; 
	viewy -= m_viewsize.y * 0.5f  ; 


	float tempX = viewx ; 
	float tempY = viewy  ; 

	//���W
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

//�����G�]�w�ثe���a�b�p�a�Ϫ��y��
void CMapControl::SetNavigationPos(float worldx, float worldz)
{
	
	//�p�⪱�a���K�Ϯy�ЩMTexture��pixel�y��
	Transform( worldx , worldz , m_roleposfortex.x , m_roleposfortex.y , 
				m_rolefortex.x , m_rolefortex.y ) ;

	assert( m_pmap != NULL ) ; 
	
	//�]�w�p�a�Ϫ��K�Ϯy��
	SetTexUVPos( m_roleposfortex.x , m_roleposfortex.y ) ;

	//�A�Q��Texture��pixel�y���ഫ��view�y��
	TexTransformView( m_rolefortex.x , m_rolefortex.y , 
		m_roleposforview.x , m_roleposforview.y ) ;

}

//�ѼơG�H���]����^���@�ɮy��(in)�A�����󪺵����y��(out)
//�����G�p��a�Ϯy�ХH�ε����y�� , �Y�o�쪺��m���b�����d��, �^��true; �Ϥ��^��false
bool CMapControl::GetWindowPos(float wx, float wz, int &x, int &y) 
{
	float texu = -1.0f;
	float texv = -1.0f;
	float screenx = -1.0f;
	float screeny = -1.0f;
	float texx = -1.0f ;
	float texy = -1.0f ;

	//Y�b�y���ഫ(���U����A���W���W�C��ù��y�Ьۤ�)
	wz = m_worldsize.y - wz ;

	//����H���b�@�ɮy�Ф���
	LimitroleInWorld( wx , wz) ;
 
	//�ഫ�y��            Textrure�K�Ϯy�� �A Texture�W��pixel�y��
	Transform( wx , wz ,  texu , texv       , texx , texy ) ;


	//float cosAngle = cos(DegreesToRads(45)); 
	//float sinAngle = sin(DegreesToRads(45)); 

	//texu -=  0.5f ; 
	//texv -=  0.5f  ; 


	//float tempX = texu ; 
	//float tempY = texv  ; 

	////���W
	//texu = (tempX * cosAngle - tempY * sinAngle) ; 
	//texv = (tempX * sinAngle + tempY * cosAngle) ; 

	//texu += 0.5f ; 
	//texv += 0.5f  ; 


	//�ˬd�H���]����^�O�_�b��ܵ����d��C
	if ( IsInside( texu , texv ) )
	{
		bool isViewed = false ; 

		//�N�H����tex's pixel�y���ഫ��ù��y��
		isViewed = TexTransformView( texx , texy , screenx , screeny ) ;



		//�֥[��������Ǯy��
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

//�����G��J�K�Ϯy�СA�ˬd���y�ЬO�_�b��ܵ����d�򤧤��C�����ܦ^��true�A�~���ܦ^��false
bool CMapControl::IsInside(float u, float v)
{
	assert( m_pmap != NULL ) ;

	return m_pmap->IsInsideForShowUV( u , v ) ;

}

//�����G����H���b�@�ɮy�а_�I �� �@�ɮy�Ъ��`�e�ΰ����d�򤧤�
void CMapControl::LimitroleInWorld( float &x , float &z )
{
	//�p�� �@�ɰ_�I
	if ( x <= m_wroldforstart.x )
	{
		x = m_wroldforstart.x ;

	}
	//�j�� �@�ɥ���
	else if ( x >= m_wroldforstart.x + m_worldsize.x )
	{
		x = m_wroldforstart.x + m_worldsize.x;
	}
	//�p�� �@�ɰ_�I
	if (z <= m_wroldforstart.y )
	{
		z = m_wroldforstart.x ;
	}
	//�j�� �@�ɥ���
	else if ( z >= m_wroldforstart.y + m_worldsize.y )
	{
		z = m_wroldforstart.y + m_worldsize.y;
	}
}


//�����G�]�w���a�b�@�ɪ��y��
void CMapControl::SetRoleWorldPos( float x , float z ) 
{
	float offsetx ;
	float offsety ;//���ʰ����q

	//Y�b�y���ഫ(���U����A���W���W�C��ù��y�Ьۤ�)
	z = m_worldsize.y - z ;


	//����a���ʤ���C�b�@�ɦa�Ͻd�򤧤�
	LimitroleInWorld( x , z ) ;

	//�p�ⰾ���q
	offsetx = x -  m_roleworldpos.x ;
	offsety = z -  m_roleworldpos.y ;


	AddRoleWorldPos( offsetx , offsety ) ;

	m_roleworldpos.x = x ;
	m_roleworldpos.y = z ;
	
	//�N�H�����@�ɮy���ন�ù��y��
	SetNavigationPos( m_roleworldpos.x , m_roleworldpos.y ) ;
}

//�����G�����H���b�ù��y�СA�٦������譱���վ�
void CMapControl::AddRoleWorldPos( float offsetx , float offsety )
{
	float temprolex ; //�Ȧs�H������m(texture�y��)
	float temproley ; 
	float moveoffsetx ;		//���ʰ����q
	float moveoffsety ;

	temprolex = m_roleposfortex.x ;
	temproley = m_roleposfortex.y ;

	m_roleworldpos.x += offsetx ;
	m_roleworldpos.y += offsety ; 


	//�p�⪱�a���K�Ϯy�ЩMTexture��pixel�y��
	Transform( m_roleworldpos.x , m_roleworldpos.y , m_roleposfortex.x , m_roleposfortex.y , 
				m_rolefortex.x , m_rolefortex.y ) ;


	//�p�ⰾ���q�G���ʫe��Texture's coord - ���ʫ᪺Texture's coord 
	moveoffsetx =  m_roleposfortex.x - temprolex;
	moveoffsety =  m_roleposfortex.y - temproley;


	///////////////////					�۾���m�ץ�			//////////////////////////
	//�h�֥[�۾���m�C�֥[���q�G�Ψӭ��s�վ�۾��������q�C�H�F��H��۾��@�P
	moveoffsetx += temprolex - m_viewposfortex.x ;
	moveoffsety += temproley - m_viewposfortex.y;


	//����camera��m
	AddUVTranslate( moveoffsetx , moveoffsety ) ;

}

