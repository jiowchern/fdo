#include "StdAfx.h"
#include "FDO_RoleStruct.h"

namespace FDO
{

//sFrame::FrameFactory sFrame::sFrameFactory;
//---------------------------------------------------------------------------
sImage::sImage()
: pTexture(NULL) ,bLoadDone(false)
{
    szTexFile[0] = '\0';

	curState = INIT ; 
	pMem = NULL ; 

}
void sImage::Clear()
{

#ifdef RESMGR 

	szTexFile[0] = '\0';

	//資源方面，交給資源管理去處理
	pTexture = NULL ;
	bLoadDone = false;
	curState = INIT ; 
	pMem = NULL ; 
#else

    szTexFile[0] = '\0';
    if( pTexture )
    {
        pTexture->Release();
        pTexture = NULL;
    }
#endif

 
}

//---------------------------------------------------------------------------
//  sFrame 結構函式
//---------------------------------------------------------------------------
sFrame::sFrame()
{
    this->Reset();
}

sFrame::~sFrame()
{
	this->Reset();
}

void sFrame::Reset()
{
    usPlayTime          = 0;
    shLayer             = 0;
    bTexInvert          = false;
    fWidth              = 0.0f;
    fHeight             = 0.0f;
    fTU1                = 0.0f;
    fTV1                = 0.0f;
    fTU2                = 0.0f;
    fTV2                = 0.0f;
    Color               = WHITE;
    fPercentX           = 50.0f;
	fPercentY           = 100.0f;
    vPosition           = VEC3_ZERO;
    vScale              = VEC3_UNIT_SCALE;
    LinearFlag          = 0;
    D3DXMatrixIdentity(&matLocal);

#ifdef RESMGR
	//if (pImage != NULL)
	//{
	//	ReleaseResource(pImage->pTexture) ;
	//}

	pImage = NULL ;
#else
    pImage = NULL;
#endif
}

void sFrame::Lerp(float s, const sFrame* rhs)
{
    if (rhs == NULL)
    {
        return;
    }

    if (LinearFlag & LINEAR::POSITION)
    {
        // TODO:
        this->vPosition = ::Lerp(s, this->vPosition, rhs->vPosition);
        //D3DXVec3Lerp
    }
    if (LinearFlag & LINEAR::ROTATION)
    {
        // TODO:
        this->vScale.z = ::Lerp(s, this->vScale.z, rhs->vScale.z);
    }
    if (LinearFlag & LINEAR::SCALE)
    {
        // TODO:
        this->vScale.x = ::Lerp(s, this->vScale.x, rhs->vScale.x);
        this->vScale.y = ::Lerp(s, this->vScale.y, rhs->vScale.y);
    }
    if (LinearFlag & LINEAR::COLOR)
    {
        // TODO:
        D3DXCOLOR src(this->Color);
        D3DXCOLOR dest(rhs->Color);
        D3DXCOLOR result;
        D3DXColorLerp(&result, &src, &dest, s);
        this->Color = result;
    }
    if (LinearFlag & LINEAR::SIZE)
    {
        // TODO:
        this->fWidth  = ::Lerp(s, this->fWidth , rhs->fWidth );
        this->fHeight = ::Lerp(s, this->fHeight, rhs->fHeight);
    }
    if (LinearFlag & LINEAR::UV)
    {
        // TODO:
        this->fTU1 = ::Lerp(s, this->fTU1, rhs->fTU1);
        this->fTV1 = ::Lerp(s, this->fTV1, rhs->fTV1);
        this->fTU2 = ::Lerp(s, this->fTU2, rhs->fTU2);
        this->fTV2 = ::Lerp(s, this->fTV2, rhs->fTV2);
    }
}

void sFrame::Clone(const sFrame *frame)
{
	usPlayTime	= frame->usPlayTime;
	shLayer		= frame->shLayer ;
	bTexInvert	= frame->bTexInvert ;
	fWidth		= frame->fWidth ;
	fHeight		= frame->fHeight;
	fTU1		= frame->fTU1;
	fTV1		= frame->fTV1;
	fTU2		= frame->fTU2;
	fTV2		= frame->fTV2;
	pImage		= frame->pImage;
	matLocal	= frame->matLocal;
	Color		= frame->Color;
	fPercentX	= frame->fPercentX;
	fPercentY	= frame->fPercentY;
	vPosition	= frame->vPosition;
	vScale		= frame->vScale;
	LinearFlag	= frame->LinearFlag;
}


void sFrame::EnableControl(bool b, int which)
{
    if (b)
    {
        LinearFlag |= which;
    }
    else
    {
        LinearFlag &= ~which;
    }
}
//
//sFrame& sFrame::operator=(const sFrame& right)
//{
//	if (this == &right) {
//		return *this;
//	}
//
//	this->usPlayTime    = right.usPlayTime;
//	this->shLayer       = right.shLayer;
//	this->bTexInvert    = right.bTexInvert;
//	this->fWidth        = right.fWidth;
//	this->fHeight       = right.fHeight;
//	this->fTU1          = right.fTU1;
//	this->fTV1          = right.fTV1;
//	this->fTU2          = right.fTU2;
//	this->fTV2          = right.fTV2;
//	this->pImage        = right.pImage;
//	this->matLocal      = right.matLocal;
//	this->Color         = right.Color;
//	this->fPercentX     = right.fPercentX;
//	this->fPercentY     = right.fPercentY;
//    this->vPosition     = right.vPosition;
//    this->vScale        = right.vScale;
//    this->LinearFlag    = right.LinearFlag;
//
//	return *this;
//}

bool sFrame::IsEnableControl(int which)
{
    return ((LinearFlag & which) != 0);
}

void sFrame::RefreshMatrix( const D3DXVECTOR3& vScale, const D3DXVECTOR3& vPosition )
{
    D3DXMATRIX matTemp;

    // 計算縮放大小
    D3DXMatrixScaling( &matLocal, vScale.x, vScale.y, 1.0f );

    // 計算自轉
	float fShiftX = -(vScale.x*fWidth*(50.0f-fPercentX)*0.01f*2.0f);
	float fShiftY = -(vScale.y*fHeight*(100.0f-fPercentY)*0.01f);
	if( bTexInvert )
	{
		fShiftX = -(vScale.x*fWidth*(fPercentX-50.0f)*0.01f*2.0f);
		fShiftY = -(vScale.y*fHeight*(100.0f-fPercentY)*0.01f);
	}
	else
	{
		fShiftX = -(vScale.x*fWidth*(50.0f-fPercentX)*0.01f*2.0f);
		fShiftY = -(vScale.y*fHeight*(100.0f-fPercentY)*0.01f);
	}
	D3DXMatrixTranslation( &matTemp, fShiftX, fShiftY, 0 );
	D3DXMatrixMultiply( &matLocal, &matLocal, &matTemp );
	D3DXMatrixRotationZ( &matTemp, vScale.z );
	D3DXMatrixMultiply( &matLocal, &matLocal, &matTemp );
	D3DXMatrixTranslation( &matTemp, -fShiftX, -fShiftY, 0 );
	D3DXMatrixMultiply( &matLocal, &matLocal, &matTemp );

    // 計算位移
    D3DXMatrixTranslation( &matTemp, vPosition.x, vPosition.y, vPosition.z );
    D3DXMatrixMultiply( &matLocal, &matLocal, &matTemp );
}

//void* sFrame::operator new (size_t)
//{
//    return sFrameFactory.Create();
//}
//
//void sFrame::operator delete (void* p)
//{
//    if (p != NULL)
//    {
//        sFrameFactory.Destroy(p);
//    }
//}

//---------------------------------------------------------------------------
//  sComponentDir 結構函式
//---------------------------------------------------------------------------
sComponentDir::sComponentDir()
{
    eDir = RD_None;
    usMaxFrameNum = 1;

	FrameList = new sFrameList ; 
}

//---------------------------------------------------------------------------
sComponentDir::~sComponentDir()
{
  //  RemoveAllFrame();

	delete FrameList; 
	FrameList = NULL ;

	Reset() ; 
}

//----------------------------------------------------------------------------
void sComponentDir::Reset()
{
    eDir = RD_None;
    usMaxFrameNum = 1;
    //RemoveAllFrame();
}

//---------------------------------------------------------------------------
void sComponentDir::RemoveAllFrame()
{
    sFrameList::iterator itEnd ( FrameList->end() );
    for( sFrameList::iterator i = FrameList->begin(); i != itEnd; ++i )
    {
        g_RoleMemoryMgr.FreeFrame( *i );
    }
    FrameList->clear();

    usMaxFrameNum = 1;
}


//----------------------------------------------------------------------------
// sRoleComponent 結構函式
//----------------------------------------------------------------------------
sRoleComponent::sRoleComponent()
: bShow(true)
{
    szComponentName[0] = '\0';
    //for( int i=0; i<8; ++i )
    //    ComponentDirs[i].eDir = (RoleDirect)i;
}

sRoleComponent::~sRoleComponent()
{
    bShow = true;
    szComponentName[0] = '\0';
}


//----------------------------------------------------------------------------
// sRoleAction 結構函式
//----------------------------------------------------------------------------
sRoleAction::sRoleAction()
{
	ComponentList = new sRoleComponentList ;

    Reset();
}

//----------------------------------------------------------------------------
sRoleAction::~sRoleAction()
{
    //RemoveAllComponent();

	Reset() ; 

	delete ComponentList ; 
	ComponentList = NULL ;
}

void sRoleAction::Reset()
{
    szActionName[0]         = '\0';
    bPlayLoop               = false;
    dwUpdateTime            = 0;
    bySoundKind             = 0;
    usHitTime1              = 0;
    usHitTime2              = 0;
    usPlaySoundTime1        = 0;
    usPlaySoundTime2        = 0;
    usPlaySoundTime3        = 0;
    usPlayParticleTime      = 0;
    szSoundFile1[0]         = '\0';
    szSoundFile2[0]         = '\0';
    szSoundFile3[0]         = '\0';
    szParticleFile[0]       = '\0';
   // RemoveAllComponent();
}

RoleComPtr sRoleAction::GetComponent(const char* pComponentName)
{
    sRoleComponentList::iterator itEnd ( ComponentList->end() );
    for( sRoleComponentList::iterator it = ComponentList->begin(); it != itEnd; ++it )
    {
        if( strcmp( (*it)->szComponentName, pComponentName ) == 0 )
            return (*it);
    }

    return NULL;
}

void sRoleAction::RemoveAllComponent()
{
    sRoleComponentList::iterator itEnd ( ComponentList->end() );
    for( sRoleComponentList::iterator it = ComponentList->begin(); it != itEnd; ++it )
    {
        g_RoleMemoryMgr.FreeComponent( *it );
    }
    ComponentList->clear();
} 

void sRoleAction::HideComponents()
{
	sRoleComponentList::iterator ComPos(ComponentList->begin());
	sRoleComponentList::const_iterator ComEnd( ComponentList->end() );
		
	for(ComPos; ComPos!=ComEnd; ++ComPos )
	{
		sRoleComponent* pCom = (*ComPos);
		pCom->bShow = false;
	}
}

unsigned short sRoleAction::GetMaxFrameNum( RoleDirect eDir )
{
    unsigned short usMaxFrameNum = 1;
    sRoleComponentList::iterator itComEnd( ComponentList->end() );
    for( sRoleComponentList::iterator it = ComponentList->begin(); it != itComEnd; ++it )
    {
        if( usMaxFrameNum < (*it)->ComponentDirs[eDir]->usMaxFrameNum )
            usMaxFrameNum = (*it)->ComponentDirs[eDir]->usMaxFrameNum;
    }

    return usMaxFrameNum;
}

} // namespace FDO